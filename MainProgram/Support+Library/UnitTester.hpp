#pragma once

namespace UnitTester
{
	inline std::random_device random_device_object;
	inline std::mt19937_64 RandomGeneraterByReallyTime(CommonSecurity::Seeds::GenerateSecureRandomNumberSeed<std::uint32_t>(random_device_object));
	inline CommonSecurity::RND::UniformIntegerDistribution<std::size_t> UniformNumberDistribution(0, 255);

	//随机生成一个长度为n的向量
	std::vector<std::uint8_t> GanerateRandomValueVector(std::size_t size)
	{
		std::vector<std::uint8_t> byte_data(size);
		for (std::size_t index = 0; index < size; index++) {
			byte_data[index] = UniformNumberDistribution(RandomGeneraterByReallyTime);
		}
		return byte_data;
	}

	class PasscoderDataDifferentialTester
	{

	protected:

		//计算两个向量之间的汉明距离（不同比特数）
		std::size_t hamming_distance(std::span<const std::uint8_t> byte_data1, std::span<const std::uint8_t> byte_data2)
		{
			std::size_t bit_distance = 0;
			for (std::size_t i = 0; i < byte_data1.size(); i++)
			{
				// 异或后计算1的个数
				bit_distance += std::popcount(static_cast<std::uint32_t>(byte_data1[i] ^ byte_data2[i]));
			}
			return bit_distance;
		}

		//将一个向量中的某一比特位反转
		std::vector<std::uint8_t> flip_bit(const std::vector<std::uint8_t>& byte_data, std::size_t position)
		{
			std::size_t byte_position = position / 8;
			std::size_t bit_positions = position % 8;

			std::vector<std::uint8_t> byte_data_copy(byte_data);

			// 异或操作反转位
			byte_data_copy[byte_position] ^= (1 << bit_positions);
			return byte_data_copy;
		}

		//计算平均比特数和方差
		std::pair<double, double> AverageBitsAndVariance(std::span<const std::size_t> data_changes)
		{
			double mean = std::accumulate(data_changes.begin(), data_changes.end(), 0.0) / data_changes.size();
			double variance = 0;
			for (std::size_t i = 0; i < data_changes.size(); i++) {
				variance += ::pow(data_changes[i] - mean, 2);
			}
			variance /= data_changes.size();

			return std::pair<double, double>{ mean, variance };
		}

		virtual std::vector<std::uint8_t> CallEncrypter(const std::vector<std::uint8_t>& PlainData, const std::vector<std::uint8_t>& Keys) = 0;

		virtual std::vector<std::uint8_t> CallDecrypter(const std::vector<std::uint8_t>& CipherData, const std::vector<std::uint8_t>& Keys) = 0;

	public:

		/*
			The Confusion and Diffusivity Tests:

			The confusion test measures how much the ciphertext changes when the plaintext is changed while keeping the key fixed. 
			The diffusivity test measures how much the ciphertext changes when the key is changed while keeping the plaintext fixed.

			To calculate the confusion and diffusivity coefficients, we can use the mean and variance of the number of bit changes in the ciphertext for each test.

			The confusion coefficient is: calculated as the mean of the number of bit changes in the ciphertext when the plaintext is changed divided by the total number of bits in the ciphertext. 
			The variance of the number of bit changes in the ciphertext for the confusion test measures how much the ciphertext changes on average when a single bit in the plaintext is changed.

			The diffusivity coefficient is: calculated as the mean of the number of bit changes in the ciphertext when the key is changed divided by the total number of bits in the ciphertext. 
			The variance of the number of bit changes in the ciphertext for the diffusivity test measures how much the ciphertext changes on average when a single bit in the key is changed.
			
			混淆性和扩散测试:

			混淆性测试衡量的是，在保持密文固定的情况下，当明文发生变化时，密文的变化程度。
			扩散性测试衡量的是，在保持明文固定的情况下，当密钥发生改变时，密文的变化程度。

			为了计算混淆系数和扩散系数，我们可以使用每个测试的密文中比特位变化数量的平均值和方差。

			混淆系数的计算方法是: 当明文发生变化时，密文中的比特位变化数的平均值除以密文中的总比特位数。
			混淆性测试中:密文比特位数变化的方差衡量了当明文中的一个比特位被改变时，密文的平均变化程度。

			扩散系数的计算方法是: 当密钥发生变化时，密文中的比特位变化数的平均值除以密文中的比特总位数。
			扩散性测试中:密文中比特位数变化的方差衡量了当密钥中的一个比特位被改变时，密文的平均变化程度。
		*/

		//测试明文数据对于密文的混淆性
		void plain_for_cipher_confusability(const std::vector<std::uint8_t>& plain, const std::vector<std::uint8_t>& key, std::size_t trials)
		{
			if((plain.size() == 0) || (key.size() == 0) || (trials == 0))
				return;

			if(key.size() < 64 || key.size() % 64 != 0)
				return;
			
			// 存储每次试验中明文改变时密文改变的比特数
			std::vector<std::size_t> plain_changes(trials);

			// 对初始明文和初始密钥进行加密，得到初始密文
			std::vector<std::uint8_t> cipher = CallEncrypter(plain, key);

			// 对每一次加密试验进行循环
			for (std::size_t i = 0; i < trials; i++)
			{
				std::cout << "\rTesting ...... " << "(" << i + 1 << "/" << trials << ")";

				// 随机选择一个比特位置，将明文中的该位反转，得到新的明文
				std::size_t plain_pos = RandomGeneraterByReallyTime() % (plain.size() * std::numeric_limits<std::uint8_t>::digits);
				std::vector<std::uint8_t> new_plain = flip_bit(plain, plain_pos);

				// 对新的明文和初始密钥进行加密，得到新的密文
				std::vector<std::uint8_t> new_cipher = CallEncrypter(new_plain, key);

				// 计算新旧密文之间的汉明距离，存入cipher_changes向量中
				std::size_t plain_change = hamming_distance(cipher, new_cipher);
				plain_changes[i] = plain_change;
			}
			std::cout << std::endl;

			// 计算密文改变时明文改变的平均比特数和方差
			auto results = AverageBitsAndVariance(plain_changes);

			std::cout << "Cipher Confusion test (Difference testing based on changed plaintext data):\n";
			std::cout << "Mean number of bit changes: " << results.first << std::endl;
			std::cout << "Variance of number of bit changes: " << results.second << std::endl;

			std::cout << "密文混淆性测试 (基于明文的变化):\n";
			std::cout << "比特位数变化的平均数量: " << results.first << std::endl;
			std::cout << "比特位数变化的方差: " << results.second << std::endl;
		}

		//测试密钥数据对于密文的扩散性
		void key_for_cipher_diffusivity(const std::vector<std::uint8_t>& plain, const std::vector<std::uint8_t>& key, std::size_t trials)
		{
			if((plain.size() == 0) || (key.size() == 0) || (trials == 0))
				return;

			if(key.size() < 64 || key.size() % 64 != 0)
				return;
			
			// 存储每次试验中密钥改变时密文改变的比特数
			std::vector<std::size_t> key_changes(trials);

			// 对初始明文和初始密钥进行加密，得到初始密文
			std::vector<std::uint8_t> cipher = CallEncrypter(plain, key);

			// 对每一次加密试验进行循环
			for (std::size_t i = 0; i < trials; i++)
			{
				std::cout << "\rTesting ...... " << "(" << i + 1 << "/" << trials << ")";

				// 随机选择一个比特位置，将明文中的该位反转，得到新的明文
				std::size_t key_pos = RandomGeneraterByReallyTime() % (key.size() * std::numeric_limits<std::uint8_t>::digits);
				std::vector<std::uint8_t> new_key = flip_bit(key, key_pos);

				// 对新的明文和初始密钥进行加密，得到新的密文
				std::vector<std::uint8_t> new_cipher = CallEncrypter(plain, new_key);

				// 计算新旧密文之间的汉明距离，存入cipher_changes向量中
				std::size_t key_change = hamming_distance(cipher, new_cipher);
				key_changes[i] = key_change;
			}
			std::cout << std::endl;

			// 计算密文改变时明文改变的平均比特数和方差
			auto results = AverageBitsAndVariance(key_changes);

			std::cout << "Cipher Diffusivity test (Difference testing based on changed key data):\n";
			std::cout << "Mean number of bit changes: " << results.first << std::endl;
			std::cout << "Variance of number of bit changes: " << results.second << std::endl;

			std::cout << "密文扩散性测试 (基于密钥的变化):\n";
			std::cout << "比特位数变化的平均数量: " << results.first << std::endl;
			std::cout << "比特位数变化的方差: " << results.second << std::endl;
		}

		//测试密文数据对于明文的混淆性
		void cipher_for_plain_confusability(const std::vector<std::uint8_t>& plain, const std::vector<std::uint8_t>& key, std::size_t trials)
		{
			if((plain.size() == 0) || (key.size() == 0) || (trials == 0))
				return;

			if(key.size() < 64 || key.size() % 64 != 0)
				return;
			
			// 存储每次试验中密文改变时明文改变的比特数
			std::vector<std::size_t> cipher_changes(trials);

			// 对初始明文和初始密钥进行加密，得到初始密文
			std::vector<std::uint8_t> cipher = CallEncrypter(plain, key);

			// 对每一次解密试验进行循环
			for (std::size_t i = 0; i < trials; i++)
			{
				std::cout << "\rTesting ...... " << "(" << i + 1 << "/" << trials << ")";

				// 随机选择一个比特位置，将密文中的该位反转，得到新的密文
				std::size_t cipher_pos = RandomGeneraterByReallyTime() % (cipher.size() * std::numeric_limits<std::uint8_t>::digits);
				std::vector<std::uint8_t> new_cipher = flip_bit(cipher, cipher_pos);

				// 对新的密文和初始密钥进行解密，得到新的明文
				std::vector<std::uint8_t> new_plain = CallDecrypter(new_cipher, key);

				// 计算新旧明文之间的汉明距离，存入cipher_changes向量中
				std::size_t cipher_change = hamming_distance(plain, new_plain);
				cipher_changes[i] = cipher_change;
			}
			std::cout << std::endl;

			// 计算密文改变时明文改变的平均比特数和方差
			auto results = AverageBitsAndVariance(cipher_changes);

			std::cout << "Plain Confusability test (Difference testing based on changed ciphertext data):\n";
			std::cout << "Mean number of bit changes: " << results.first << std::endl;
			std::cout << "Variance of number of bit changes: " << results.second << std::endl;

			std::cout << "明文混淆性测试 (基于密文的变化):\n";
			std::cout << "比特位数变化的平均数量: " << results.first << std::endl;
			std::cout << "比特位数变化的方差: " << results.second << std::endl;
		}

		//测试密钥数据对于明文的扩散性
		void key_for_plain_diffusivity(const std::vector<std::uint8_t>& plain, const std::vector<std::uint8_t>& key, std::size_t trials)
		{
			if((plain.size() == 0) || (key.size() == 0) || (trials == 0))
				return;

			if(key.size() < 64 || key.size() % 64 != 0)
				return;

			// 存储每次试验中密钥改变时明文改变的比特数
			std::vector<std::size_t> key_changes(trials);

			// 对初始明文和初始密钥进行加密，得到初始密文
			std::vector<std::uint8_t> cipher = CallEncrypter(plain, key);

			// 对每一次解密试验进行循环
			for (std::size_t i = 0; i < trials; i++)
			{
				std::cout << "\rTesting ...... " << "(" << i + 1 << "/" << trials << ")";

				// 随机选择一个比特位置，将密钥中的该位反转，得到新的密钥
				std::size_t key_pos = RandomGeneraterByReallyTime() % (key.size() * std::numeric_limits<std::uint8_t>::digits);
				std::vector<std::uint8_t> new_key = flip_bit(key, key_pos);

				// 对密文和新密钥进行解密，得到新的明文
				std::vector<std::uint8_t> new_plain = CallDecrypter(cipher, new_key);

				// 计算新旧明文之间的汉明距离，存入key_changes向量中
				std::size_t key_change = hamming_distance(plain, new_plain);
				key_changes[i] = key_change;
			}
			std::cout << std::endl;

			// 计算密钥改变时明文改变的平均比特数和方差
			auto results = AverageBitsAndVariance(key_changes);

			std::cout << "Plain Diffusivity test (Difference testing based on changed key data):\n";
			std::cout << "Mean number of bit changes: " << results.first << std::endl;
			std::cout << "Variance of number of bit changes: " << results.second << std::endl;

			std::cout << "明文扩散性测试 (基于密钥的变化):\n";
			std::cout << "比特位数变化的平均数量: " << results.first << std::endl;
			std::cout << "比特位数变化的方差: " << results.second << std::endl;
		}

		PasscoderDataDifferentialTester() = default;
		virtual ~PasscoderDataDifferentialTester() = default;
	};

	class TesterMyRivestCipher4Star : public PasscoderDataDifferentialTester
	{

	private:

		std::vector<std::uint8_t> CallEncrypter(const std::vector<std::uint8_t>& PlainData, const std::vector<std::uint8_t>& Keys) override final
		{
			StreamDataCryption::RC4::RivestCipher4Star RC4Star;

			RC4Star.KeyScheduling(Keys);
			std::vector<std::uint8_t> Subkeys = RC4Star.GenerateKeyStream(PlainData.size());

			std::vector<std::uint8_t> CipherData(PlainData.size(), 0);

			std::ranges::transform
			(
				PlainData.begin(),
				PlainData.end(),
				Subkeys.begin(),
				Subkeys.end(),
				CipherData.begin(),
				[](const std::uint64_t& left, const std::uint64_t& right)
				{
					return left ^ right;
				}
			);

			return CipherData;
		}

		std::vector<std::uint8_t> CallDecrypter(const std::vector<std::uint8_t>& CipherData, const std::vector<std::uint8_t>& Keys) override final
		{
			StreamDataCryption::RC4::RivestCipher4Star RC4Star;

			RC4Star.KeyScheduling(Keys);
			std::vector<std::uint8_t> Subkeys = RC4Star.GenerateKeyStream(CipherData.size());

			std::vector<std::uint8_t> PlainData(CipherData.size(), 0);

			std::ranges::transform
			(
				CipherData.begin(),
				CipherData.end(),
				Subkeys.begin(),
				Subkeys.end(),
				PlainData.begin(),
				[](const std::uint64_t& left, const std::uint64_t& right)
				{
					return left ^ right;
				}
			);

			return PlainData;
		}

	public:

		explicit TesterMyRivestCipher4Star()
			: PasscoderDataDifferentialTester()
		{
			
		}
		virtual ~TesterMyRivestCipher4Star() = default;
	};

	/*
		Autocorrelation is a method that can be used to analyze the randomness of a sequence of numbers or bytes.
		It measures how similar a sequence is to a delayed version of itself, and can be used to identify patterns or repeating structures in the data.
	*/
	double ByteDataAutoCorrelation(const std::vector<std::uint8_t>& data, std::size_t round)
	{
		std::vector<double> auto_correlation_datas(round+1, 0.0);

		// Compute the mean of the data
		double mean = 0.0;
		for (std::uint8_t x : data)
		{
			mean += static_cast<double>(x);
		}
		mean /= static_cast<double>(data.size());

		// Compute the variance of the data
		double var = 0.0;
		for (std::uint8_t x : data)
		{
			var += (static_cast<double>(x) - mean) * (static_cast<double>(x) - mean);
		}
		var /= static_cast<double>(data.size());

		// Compute the autocorrelation for each lag value
		for (std::size_t lag = 0; lag <= round; lag++)
		{
			double sum = 0.0;
			for (std::size_t i = 0; i < data.size() - lag - 1; i++) {
				sum += (static_cast<double>(data[i]) - mean) * (static_cast<double>(data[i+lag]) - mean);
			}
			auto_correlation_datas[lag] = sum / ((data.size() - lag - 1) * var);
		}

		 // Compute the average autocorrelation
		double average = 0.0;
		for (double value : auto_correlation_datas) {
			average += value;
		}
		average /= static_cast<double>(auto_correlation_datas.size());

		return average;
	}

	/*
		This function takes a ciphertext as input and returns a dictionary containing the frequency of each byte in the ciphertext, expressed as a percentage of the total number of bytes.
		You can use this function to compare the frequency distribution of the ciphertext to the expected distribution for random data.
		If the distribution of the ciphertext is significantly different from the expected distribution, this may indicate that the ciphertext is not sufficiently random.
	*/
	void ByteFrequencyAnalysis(std::span<std::uint8_t> data)
	{
		// Initialize an array to count the frequency of each byte
		std::array<std::uint32_t, 256> freq = {0};

		// Count the frequency of each byte in the input data
		for (size_t i = 0; i < data.size(); i++)
		{
			freq[data[i]]++;
		}

		// Print the frequency of each byte in the input data
		for (std::size_t i = 0; i < 256; i++)
		{
			if (freq[i] > 0) {
				std::cout << "Byte 0x" << std::hex << i << ": " << freq[i] << std::endl;
			}
		}
	}

	template <typename DataType> 
	requires CommonToolkit::IsIterable::IsIterable<DataType>
	double ShannonInformationEntropy(DataType& data)
	{
		//H
		double entropy { 0.0 };

		#if 1

		std::size_t frequencies_count { 0 };

		std::map<std::ranges::range_value_t<DataType>, std::size_t> map;

		for (const auto& item : data)
		{
			map[item]++;
		}

		std::size_t size = std::size(data);

		for (auto iterator = map.cbegin(); iterator != map.cend(); ++iterator)
		{
			double probability_x = static_cast<double>(iterator->second) / static_cast<double>(size);
			entropy -= probability_x * std::log2(probability_x);
			++frequencies_count;
		}

		if (frequencies_count > 256)
		{
			return -1.0;
		}

		return entropy < 0.0 ? -entropy : entropy;

		#else

		DataType copy_data(data);

		std::sort(std::begin(copy_data), std::end(copy_data));

		const std::size_t copy_data_size = std::size(copy_data);
		std::size_t hide_function = 1;
		for (std::size_t index = 1; index < copy_data_size; ++index)
		{
			if (copy_data[index] == copy_data[index - 1])
				++hide_function;
			else
			{
				const double hide_function_size = static_cast<double>(hide_function) / copy_data_size;
				entropy -= hide_function_size * std::log2(hide_function_size);
				hide_function = 1;
			}
		}

		return entropy;

		#endif
	}
	
	template<typename ThisArgumentType>
	requires std::constructible_from< std::span<std::byte>, ThisArgumentType> || std::constructible_from< std::span<std::uint8_t>, ThisArgumentType>
	void UsedAlgorithmByteDataDifferences(std::string AlgorithmName, ThisArgumentType&& BeforeByteData, ThisArgumentType&& AfterByteData)
	{
		std::size_t DifferentByteCounter = 0;

		std::size_t CountBitOneA = 0;
		std::size_t CountBitOneB = 0;

		for
		(
			auto IteratorBegin = BeforeByteData.begin(), IteratorEnd = BeforeByteData.end(),
			IteratorBegin2 = AfterByteData.begin(), IteratorEnd2 = AfterByteData.end();
			IteratorBegin != IteratorEnd && IteratorBegin2 != IteratorEnd2;
			++IteratorBegin, ++IteratorBegin2
		)
		{
			if(*IteratorBegin != *IteratorBegin2)
				++DifferentByteCounter;

				CountBitOneA += std::popcount( static_cast<std::uint8_t>(*IteratorBegin) );
				CountBitOneB += std::popcount( static_cast<std::uint8_t>(*IteratorBegin2) );
		}

		std::cout << "Applying this symmetric encryption and decryption algorithm " << "[" << AlgorithmName << "]" << std::endl;
		std::cout << "The result is that a difference of ("<< DifferentByteCounter << ") bytes happened !" << std::endl;
		std::cout << "Difference ratio is: " << static_cast<double>(DifferentByteCounter * 100.0) / static_cast<double>( BeforeByteData.size() ) << "%" << std::endl;

		std::cout << "The result is that a hamming distance difference of ("  << ( CountBitOneA > CountBitOneB ? "+" : "-" ) << ( CountBitOneA > CountBitOneB ? CountBitOneA - CountBitOneB : CountBitOneB - CountBitOneA ) <<  ") bits happened !" << std::endl;
		std::cout << "Difference ratio is: " << static_cast<double>(CountBitOneA * 100.0) / static_cast<double>(CountBitOneB) << "%" << std::endl;
	}

	inline void Test_StreamCryptograph_MyRivestCipher4Star()
	{
		#if 0

		// 随机生成一个初始明文和初始密钥
		std::vector<std::uint8_t> PlainData = GanerateRandomValueVector(1048579);
		std::vector<std::uint8_t> Keys = GanerateRandomValueVector(5120);

		#else

		std::vector<std::uint8_t> PlainData(1048579, std::uint8_t{0x00});
		std::vector<std::uint8_t> Keys(5120, std::uint8_t{0x01});

		#endif

		TesterMyRivestCipher4Star Tester_RC4Star{};

		//Test Encryption
		Tester_RC4Star.plain_for_cipher_confusability(PlainData, Keys, 1024);
		Tester_RC4Star.key_for_cipher_diffusivity(PlainData, Keys, 1024);

		//Test Decryption
		Tester_RC4Star.cipher_for_plain_confusability(PlainData, Keys, 1024);
		Tester_RC4Star.key_for_plain_diffusivity(PlainData, Keys, 1024);
	}

	inline void Show_StreamCryptograph_MyRivestCipher4Star()
	{
		std::mt19937 PRNG(1);
		std::vector<std::uint8_t> ByteOriginalKeys;
		std::vector<std::uint8_t> ByteKeyStream;
		for(std::uint32_t count = 0; count < 256; count++)
			ByteOriginalKeys.push_back(PRNG());

		std::uint32_t ByteCounter = 0;
		const auto backup_io_format_flags = std::cout.flags();

		StreamDataCryption::RC4::RivestCipher4 RC4_StreamCipher;
		RC4_StreamCipher.KeyScheduling( ByteOriginalKeys );
		ByteKeyStream = RC4_StreamCipher.GenerateKeyStream( 512 );
		std::cout << "Key Stream Byte Data With RivestCipher4:" << std::endl;
		for ( const auto& ByteKey : ByteKeyStream )
		{
			std::cout << "0x" << std::setfill( '0' ) << std::setw( sizeof( std::uint8_t ) * 2 ) << std::hex << static_cast<std::uint32_t>( ByteKey ) << ',' << '\t';
			++ByteCounter;
			if ( ByteCounter % 16 == 0 )
			{
				std::cout << '\n';
			}
		}

		ByteCounter = 0;
		std::cout << "--------------------------------" << std::endl;

		StreamDataCryption::RC4::RivestCipher4A RC4A_StreamCipher;
		RC4A_StreamCipher.KeyScheduling( ByteOriginalKeys );
		ByteKeyStream = RC4A_StreamCipher.GenerateKeyStream( 512 );
		std::cout << "Key Stream Byte Data With RivestCipher4A:" << std::endl;
		for ( const auto& ByteKey : ByteKeyStream )
		{
			std::cout << "0x" << std::setfill( '0' ) << std::setw( sizeof( std::uint8_t ) * 2 ) << std::hex << static_cast<std::uint32_t>( ByteKey ) << ',' << '\t';
			++ByteCounter;
			if ( ByteCounter % 16 == 0 )
			{
				std::cout << '\n';
			}
		}

		ByteCounter = 0;
		std::cout << "--------------------------------" << std::endl;

		StreamDataCryption::RC4::RivestCipher4Plus RC4_Plus_StreamCipher;
		RC4_Plus_StreamCipher.KeyScheduling( ByteOriginalKeys );
		ByteKeyStream = RC4_Plus_StreamCipher.GenerateKeyStream( 512 );
		std::cout << "Key Stream Byte Data With RivestCipher4 Plus:" << std::endl;
		for ( const auto& ByteKey : ByteKeyStream )
		{
			std::cout << "0x" << std::setfill( '0' ) << std::setw( sizeof( std::uint8_t ) * 2 ) << std::hex << static_cast<std::uint32_t>( ByteKey ) << ',' << '\t';
			++ByteCounter;
			if ( ByteCounter % 16 == 0 )
			{
				std::cout << '\n';
			}
		}

		ByteCounter = 0;
		std::cout << "--------------------------------" << std::endl;

		StreamDataCryption::RC4::RivestCipher4_VMPC RC4_VMPC_StreamCipher;
		RC4_VMPC_StreamCipher.KeyScheduling( ByteOriginalKeys );
		ByteKeyStream = RC4_VMPC_StreamCipher.GenerateKeyStream( 512 );
		std::cout << "Key Stream Byte Data With RivestCipher4 VMPC:" << std::endl;
		for ( const auto& ByteKey : ByteKeyStream )
		{
			std::cout << "0x" << std::setfill( '0' ) << std::setw( sizeof( std::uint8_t ) * 2 ) << std::hex << static_cast<std::uint32_t>( ByteKey ) << ',' << '\t';
			++ByteCounter;
			if ( ByteCounter % 16 == 0 )
			{
				std::cout << '\n';
			}
		}

		ByteCounter = 0;
		std::cout << "--------------------------------" << std::endl;

		StreamDataCryption::RC4::RivestCipher4_Spritz RC4_Spritz_StreamCipher;
		RC4_Spritz_StreamCipher.KeyScheduling( ByteOriginalKeys );
		ByteKeyStream = RC4_Spritz_StreamCipher.GenerateKeyStream( 512 );
		std::cout << "Key Stream Byte Data With RivestCipher4 Spritz:" << std::endl;
		for ( const auto& ByteKey : ByteKeyStream )
		{
			std::cout << "0x" << std::setfill( '0' ) << std::setw( sizeof( std::uint8_t ) * 2 ) << std::hex << static_cast<std::uint32_t>( ByteKey ) << ',' << '\t';
			++ByteCounter;
			if ( ByteCounter % 16 == 0 )
			{
				std::cout << '\n';
			}
		}

		ByteCounter = 0;
		std::cout << "--------------------------------" << std::endl;

		StreamDataCryption::RC4::RivestCipher4Star RC4_Star_StreamCipher;
		RC4_Star_StreamCipher.KeyScheduling( ByteOriginalKeys );
		ByteKeyStream = RC4_Star_StreamCipher.GenerateKeyStream( 512 );
		std::cout << "Key Stream Byte Data With RivestCipher4 Star:" << std::endl;
		for ( const auto& ByteKey : ByteKeyStream )
		{
			std::cout << "0x" << std::setfill( '0' ) << std::setw( sizeof( std::uint8_t ) * 2 ) << std::hex << static_cast<std::uint32_t>( ByteKey ) << ',' << '\t';
			++ByteCounter;
			if ( ByteCounter % 16 == 0 )
			{
				std::cout << '\n';
			}
		}

		ByteCounter = 0;
		std::cout << "--------------------------------" << std::endl;

		std::cout.flags(backup_io_format_flags);
	}
}