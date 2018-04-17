#include <iostream>

#include "TransferImage_Utils.h"
#include "TransferImage.h"


void testReceiver(void)
{
	std::cout << "<hi from receiver>\n";

	try {
		//init the connection and possibly wait for the header information
		imgParams_t imgParams;
		connectionParams_t cnnParams;
		//StartReceivingOneImage(imgParams,cnnParams,54545,10);
		StartRequestingOneImage(imgParams,cnnParams,"localhost:54545",10);

		//aha, so this is what we will receive -- do what you need to get ready for that
		std::cout << "Going to receive an image: ";
		for (int i=1; i < imgParams.dim; ++i)
		          std::cout << imgParams.sizes[i-1] << " x ";
		std::cout << imgParams.sizes[imgParams.dim-1] << "\n";
		std::cout << "VT     : " << imgParams.voxelType << "\n"
		          << "backend: " << imgParams.backendType << "\n";

		std::cout << "array length : " << imgParams.howManyVoxels() << " voxels\n";
		std::cout << "array memSize: " << imgParams.howManyBytes() << " Bytes\n";

		//get metadata
		std::list<std::string> metaData;
		ReceiveMetadata(cnnParams,metaData);

		std::cout << "--metadata--\n";
		std::list<std::string>::const_iterator it = metaData.begin();
		while (it != metaData.end())
		{
			std::cout << *it << "\n";
			it++;
		}
		std::cout << "--metadata--\n";

		//prepare an array to hold the pixel data... we will cast it later
		//(or we would need to create here a template function in which
		// the correct-type 'data' variable would be created and used)
		char* data = new char[imgParams.howManyBytes()];

		//now, fill the image data array
		switch (imgParams.enumVoxelType())
		{
			case imgParams::voxelTypes::Byte:
				TransmitOneImage(cnnParams,imgParams,(char*)data);
				break;
			case imgParams::voxelTypes::UnsignedByte:
				TransmitOneImage(cnnParams,imgParams,(unsigned char*)data);
				break;

			case imgParams::voxelTypes::Short:
				TransmitOneImage(cnnParams,imgParams,(signed short*)data);
				break;
			case imgParams::voxelTypes::UnsignedShort:
				TransmitOneImage(cnnParams,imgParams,(unsigned short*)data);
				break;

			case imgParams::voxelTypes::Int:
				TransmitOneImage(cnnParams,imgParams,(signed int*)data);
				break;
			case imgParams::voxelTypes::UnsignedInt:
				TransmitOneImage(cnnParams,imgParams,(unsigned int*)data);
				break;

			case imgParams::voxelTypes::Long:
				TransmitOneImage(cnnParams,imgParams,(signed long*)data);
				break;
			case imgParams::voxelTypes::UnsignedLong:
				TransmitOneImage(cnnParams,imgParams,(unsigned long*)data);
				break;

			case imgParams::voxelTypes::Float:
				TransmitOneImage(cnnParams,imgParams,(float*)data);
				break;
			case imgParams::voxelTypes::Double:
				TransmitOneImage(cnnParams,imgParams,(double*)data);
				break;

			default:
				std::cout << "ups, not ready yet for voxel type: " << imgParams.voxelType << "\n";
		}

		//close the connection, calls also cnnParams.clear()
		FinishReceivingOneImage(cnnParams);

		//print first 20 values just to see that something has been transmitted
		for (long i=0; i < 20; ++i)
			std::cout << (int)data[i] << ",";
		std::cout << std::endl;
/*
		//next slice
		for (long i=0; i < 20; ++i)
			std::cout << (int)data[imgParams.sizes[0]*imgParams.sizes[1]+i] << ",";
		std::cout << std::endl;
*/

		//free the memory! we're not in Java :)
		imgParams.clear();
		delete[] data;
	}
	catch (std::exception* e)
	{
		std::cout << "Transmission problem: " << e->what() << std::endl;
	}

	std::cout << "</hi from receiver>\n";
}


void testSender(void)
{
	std::cout << "<hi from sender>\n";

	try {
		//init the connection and possibly wait for the header information
		imgParams_t imgParams;

		//setup testing image
		imgParams.dim = 3;
		imgParams.sizes = new int[3];
		imgParams.sizes[0] = 610;
		imgParams.sizes[1] = 590;
		imgParams.sizes[2] = 3;
		imgParams.voxelType = std::string("UnsignedShortType");
		imgParams.backendType = std::string("PlanarImg"); //IMPORTANT

		//aha, so this is what we will send
		std::cout << "Going to send an image: ";
		for (int i=1; i < imgParams.dim; ++i)
		          std::cout << imgParams.sizes[i-1] << " x ";
		std::cout << imgParams.sizes[imgParams.dim-1] << "\n";
		std::cout << "VT     : " << imgParams.voxelType << "\n"
		          << "backend: " << imgParams.backendType << "\n";

		std::cout << "array length : " << imgParams.howManyVoxels() << " voxels\n";
		std::cout << "array memSize: " << imgParams.howManyBytes() << " Bytes\n";

		//prepare the "image" to be sent away
		unsigned short* data = new unsigned short[imgParams.howManyVoxels()];
		for (int i=0; i < imgParams.howManyVoxels(); ++i) data[i]=0;
		for (int i=0; i < 30; ++i)
			data[i + i*610]=data[5 + i + i*610]=20;

		connectionParams_t cnnParams;
		//StartSendingOneImage(imgParams,cnnParams,"localhost:54546",10);
		StartServingOneImage(imgParams,cnnParams,54545,10);

		//set and send metadata
		std::list<std::string> metaData;
		//IMPORTANT two lines: 'imagename' and 'some name with allowed whitespaces'
		metaData.push_back(std::string("imagename"));
		metaData.push_back(std::string("sent from C++ world"));
		SendMetadata(cnnParams,metaData);

		//send the raw pixel image data
		TransmitOneImage(cnnParams,imgParams,data);

		//close the connection, calls also cnnParams.clear()
		FinishSendingOneImage(cnnParams);

		//free the memory! we're not in Java :)
		imgParams.clear();
		delete[] data;
	}
	catch (std::exception* e)
	{
		std::cout << "Transmission problem: " << e->what() << std::endl;
	}

	std::cout << "</hi from sender>\n";
}


int main(void)
{
	//testReceiver();
	testSender();

	return (0);
}
