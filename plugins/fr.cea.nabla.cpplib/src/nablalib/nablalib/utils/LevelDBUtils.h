/*******************************************************************************
 * Copyright (c) 2021 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
#ifndef NABLALIB_UTILS_LEVELDBUTILS_H_
#define NABLALIB_UTILS_LEVELDBUTILS_H_

#include <iostream>
#include <string>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

namespace nablalib::utils
{
	struct DataDescriptor
	{
		int dataTypeBytes;
		int dataSizes[4];
	};

	struct DataDiff
	{
		int nbErrors;
		double maxError;
		int maxErrorIndex;
	};

	static std::string descriptorSuffix = "_descriptor";

	inline void putDBDescriptor(leveldb::WriteBatch* batch, const std::string dataName, int dataTypeBytes, std::vector<size_t> dataSizes)
	{
		DataDescriptor dataDescriptor;
		memset(&dataDescriptor.dataSizes, 0, 4 * sizeof(int));
		for (size_t i = 0; i < dataSizes.size(); i++)
			dataDescriptor.dataSizes[i] = dataSizes[i];
		dataDescriptor.dataTypeBytes = dataTypeBytes;
		batch->Put(dataName, leveldb::Slice((const char*)&dataDescriptor, sizeof(DataDescriptor)));
	}

	template <typename T>
	void putDBValue(leveldb::WriteBatch* batch, const std::string dataName, T& dataValue)
	{
		int size;
		bool mustDelete = false;
		const char* array = serialize(dataValue, size, mustDelete);
		batch->Put(dataName, leveldb::Slice(array, size));
		if (mustDelete)
			delete []array;
	}

	inline bool endsWith(std::string const &fullString, std::string const &ending)
	{
		if (fullString.size() >= ending.size())
			return (0 == fullString.compare (fullString.size() - ending.size(), ending.size(), ending));
		else
			return false;
	}

	inline std::string getMismatchIndexes(int dataSizes[4], int mismatchIndex)
	{
		std::stringstream ss;
		if (dataSizes[1] == 0)
			ss << "[" << mismatchIndex << "]";
		else if (dataSizes[2] == 0)
			ss << "[" << mismatchIndex / dataSizes[1] << "]" << "[" << mismatchIndex % dataSizes[1] << "]";
		else if (dataSizes[3] == 0)
			ss << "[" << mismatchIndex / (dataSizes[1] * dataSizes[2]) << "]"
			<< "[" << (mismatchIndex % (dataSizes[1] * dataSizes[2])) / dataSizes[2] << "]"
			<< "[" << (mismatchIndex % (dataSizes[1] * dataSizes[2])) % dataSizes[2] << "]";
		else ss << "[" << mismatchIndex / (dataSizes[1] * dataSizes[2] * dataSizes[3] ) << "]"
				<< "[" << (mismatchIndex % (dataSizes[1] * dataSizes[2] * dataSizes[3] )) / ( dataSizes[2] * dataSizes[3] ) << "]"
				<< "[" << ((mismatchIndex % (dataSizes[1] * dataSizes[2] * dataSizes[3] )) % ( dataSizes[2] * dataSizes[3] )) / dataSizes[3] << "]"
				<< "[" << ((mismatchIndex % (dataSizes[1] * dataSizes[2] * dataSizes[3] )) % ( dataSizes[2] * dataSizes[3] )) % dataSizes[3] << "]";
		return ss.str();
	}

	inline bool isScalar(int dataSizes[4])
	{
		return dataSizes[0] == 0;
	}

	template <typename T>
	inline DataDiff compareDataT(leveldb::Slice value, leveldb::Slice ref, size_t bytes)
	{
		DataDiff dataDiff {0, 0.0, 0};
		for (size_t i = 0; i <= value.size()- bytes && i <= ref.size() - bytes; i = i + bytes)
		{
			char* valData = value.ToString().substr(i, bytes).data();
			T val = *(T*)valData;
			char* refData = ref.ToString().substr(i, bytes).data();
			T ref = *(T*)refData;
			if (val != ref)
			{
				dataDiff.nbErrors ++;
				if (abs(val -ref) > dataDiff.maxError)
				{
					dataDiff.maxErrorIndex = i;
					dataDiff.maxError = abs(val -ref);
				}
			}
		}
		return dataDiff;
	}

	inline DataDiff compareData(leveldb::Slice value, leveldb::Slice ref, size_t bytes)
	{
		if (bytes == sizeof(double))
			return compareDataT<double>(value, ref, bytes);
		else if (bytes == sizeof(int))
			return compareDataT<int>(value, ref, bytes);
		return DataDiff {0, 0.0, 0};
	}

	inline bool compareDB(const std::string& current, const std::string& ref)
	{
		// Final result
		bool result = true;

		// Loading ref DB
		leveldb::DB* db_ref;
		leveldb::Options db_options_ref;
		db_options_ref.create_if_missing = false;
		leveldb::Status status = leveldb::DB::Open(db_options_ref, ref, &db_ref);
		if (!status.ok())
		{
			std::cerr << "No ref database to compare with ! Looking for " << ref << std::endl;
			return false;
		}
		leveldb::Iterator* it_ref = db_ref->NewIterator(leveldb::ReadOptions());

		// Loading current DB
		leveldb::DB* db;
		leveldb::Options db_options;
		db_options.create_if_missing = false;
		status = leveldb::DB::Open(db_options, current, &db);
		assert(status.ok());

		leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

		// Results comparison
		std::cerr << "# Comparing results ..." << std::endl;
		for (it_ref->SeekToFirst(); it_ref->Valid(); it_ref->Next()) {
			std::string key = it_ref->key().ToString();
			if (!endsWith(key, descriptorSuffix))
			{
				std::string dataValueAsString;
				auto status = db->Get(leveldb::ReadOptions(), key, &dataValueAsString);
				leveldb::Slice value = leveldb::Slice(dataValueAsString);

				if (status.IsNotFound()) {
					std::cerr << "ERROR - Key : " << key << " not found." << std::endl;
					result = false;
				}
				else {
					if (value.compare(it_ref->value()) == 0)
						std::cerr << key << ": " << "OK" << std::endl;
					else
					{
						result = false;
						std::string dataDescriptorKey = key + descriptorSuffix;
						std::string dataDescriptorAsString;
						assert(db_ref->Get(leveldb::ReadOptions(), key + descriptorSuffix, &dataDescriptorAsString).ok());
						DataDescriptor* dataDescriptor = (DataDescriptor*)(dataDescriptorAsString.data());
						int bytes = dataDescriptor->dataTypeBytes;

						DataDiff dataDiff = compareData(value, it_ref->value(), bytes);
						int nbVals = dataValueAsString.size() / bytes;
						std::cerr << key << ": " << "ERRORS " << dataDiff.nbErrors << "/"<<  nbVals << std::endl;

						if (isScalar(dataDescriptor->dataSizes))
						{
							if (bytes == sizeof(int))
								std::cerr << "	Error: expected " << *(int*)it_ref->value().data() << " but was " << *(int*)dataValueAsString.data() << std::endl;
							else if (bytes == sizeof(double))
							{
								double valAsDouble = *(double*)dataValueAsString.data();
								double refAsDouble = *(double*)it_ref->value().data();
								std::cerr << "	Error: expected " << *(double*)it_ref->value().data() << " but was " << *(double*)dataValueAsString.data() << " (Diff = " << valAsDouble - refAsDouble << ")" << std::endl;
							}
						}
						else
						{
							std::string refValueAsString(it_ref->value().ToString());
							std::string indexes = getMismatchIndexes(dataDescriptor->dataSizes, dataDiff.maxErrorIndex / bytes);
							if (bytes == sizeof(int))
								std::cerr << "	Error max in "<< key << indexes << ": expected " << *(int*)refValueAsString.substr(dataDiff.maxErrorIndex, bytes).data() << " but was " << *(int*)dataValueAsString.substr(dataDiff.maxErrorIndex, bytes).data() << std::endl;
							else if (bytes == sizeof(double))
							{
								double valAsDouble = *(double*)dataValueAsString.substr(dataDiff.maxErrorIndex, bytes).data();
								double refAsDouble = *(double*)refValueAsString.substr(dataDiff.maxErrorIndex, bytes).data();
								std::cerr << "	Error max in "<< key << indexes << ": expected " << refAsDouble << " but was " << valAsDouble << " (Diff = " << valAsDouble - refAsDouble << ")" << std::endl;
							}
						}
					}
				}
			}
		}

		// looking for key in the db that are not in the ref (new variables)
		for (it->SeekToFirst(); it->Valid(); it->Next()) {
			std::string key = it->key().data();
			std::string value;
			if (db_ref->Get(leveldb::ReadOptions(), it->key(), &value).IsNotFound()) {
				std::cerr << "ERROR - Key : " << key << " can not be compared (not present in the ref)." << std::endl;
				result = false;
			}
		}

		// Freeing memory
		delete db;
		delete db_ref;
		return result;
	}
}
#endif /* NABLALIB_UTILS_LEVELDBUTILS_H_ */
