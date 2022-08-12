/*
    Copyright (c) 2018-2022 Xavier Leclercq
    Released under the MIT License
    See https://github.com/diplodocusdb/physical-storage/blob/main/LICENSE.txt
*/

#include "PageTests.h"
#include "DiplodocusDB/PhysicalStorage/Page.hpp"

using namespace DiplodocusDB::PhysicalStorage;
using namespace Ishiko;

PageTests::PageTests(const TestNumber& number, const TestContext& context)
    : TestSequence(number, "Page tests", context)
{
    append<HeapAllocationErrorsTest>("ConstructorTest1 test 1", ConstructorTest1);
    append<HeapAllocationErrorsTest>("read test 1", ReadTest1);
    append<HeapAllocationErrorsTest>("read test 2", ReadTest2);
    append<HeapAllocationErrorsTest>("read test 3", ReadTest3);
    append<HeapAllocationErrorsTest>("read test 4", ReadTest4);
    append<HeapAllocationErrorsTest>("read test 5", ReadTest5);
    append<HeapAllocationErrorsTest>("write test 1", WriteTest1);
    append<HeapAllocationErrorsTest>("get test 1", GetTest1);
    append<HeapAllocationErrorsTest>("insert test 1", InsertTest1);
    append<HeapAllocationErrorsTest>("insert test 2", InsertTest2);
    append<HeapAllocationErrorsTest>("insert test 3", InsertTest3);
    append<HeapAllocationErrorsTest>("erase test 1", EraseTest1);
    append<HeapAllocationErrorsTest>("erase test 2", EraseTest2);
    append<HeapAllocationErrorsTest>("erase test 3", EraseTest3);
    append<HeapAllocationErrorsTest>("moveTo test 1", MoveToTest1);
    append<HeapAllocationErrorsTest>("moveTo test 2", MoveToTest2);
}

void PageTests::ConstructorTest1(Test& test)
{
    Page page{0};

    ISHIKO_TEST_PASS();
}

/// Tests reading an empty page.
void PageTests::ReadTest1(Test& test)
{
    boost::filesystem::path inputPath = test.context().getDataPath("PageTests_ReadTest1.dpdb");
    
    Page page{0};

    Error error;
    std::ifstream input(inputPath.c_str(), std::fstream::binary);
    page.read(input, error);
    
    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page.dataSize(), 0);
    ISHIKO_TEST_FAIL_IF_NEQ(page.availableSpace(), 4080);
    ISHIKO_TEST_PASS();
}

/// Tests reading a page containing 6 bytes of data.
void PageTests::ReadTest2(Test& test)
{
    boost::filesystem::path inputPath = test.context().getDataPath("PageTests_ReadTest2.dpdb");

    Page page{0};

    Error error;
    std::ifstream input(inputPath.c_str(), std::fstream::binary);
    page.read(input, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page.dataSize(), 6);
    ISHIKO_TEST_FAIL_IF_NEQ(page.availableSpace(), 4074);
    ISHIKO_TEST_PASS();
}

/// Tests reading the second page.
void PageTests::ReadTest3(Test& test)
{
    boost::filesystem::path inputPath = test.context().getDataPath("PageTests_ReadTest3.dpdb");

    Page page{1};

    Error error;
    std::ifstream input(inputPath.c_str(), std::fstream::binary);
    page.read(input, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page.dataSize(), 10);
    ISHIKO_TEST_FAIL_IF_NEQ(page.availableSpace(), 4070);
    ISHIKO_TEST_PASS();
}

/// Tests reading a page that doesn't exist.
void PageTests::ReadTest4(Test& test)
{
    boost::filesystem::path inputPath = test.context().getDataPath("PageTests_ReadTest4.dpdb");

    Page page{1};

    Error error;
    std::ifstream input(inputPath.c_str(), std::fstream::binary);
    page.read(input, error);

    ISHIKO_TEST_FAIL_IF_NOT(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page.dataSize(), 0);
    ISHIKO_TEST_FAIL_IF_NEQ(page.availableSpace(), 4080);
    ISHIKO_TEST_PASS();
}

/// Tests reading a page that is incomplete, this should be impossible unless the file has been truncated.
void PageTests::ReadTest5(Test& test)
{
    boost::filesystem::path inputPath = test.context().getDataPath("PageTests_ReadTest5.dpdb");

    Page page{0};

    Error error;
    std::ifstream input(inputPath.c_str(), std::fstream::binary);
    page.read(input, error);

    ISHIKO_TEST_FAIL_IF_NOT(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page.dataSize(), 0);
    ISHIKO_TEST_FAIL_IF_NEQ(page.availableSpace(), 4080);
    ISHIKO_TEST_PASS();
}

void PageTests::WriteTest1(Test& test)
{
    const char* outputName = "PageTests_WriteTest1.dpdb";
    
    Page page{0};
    page.init();

    Error error;
    std::ofstream output(test.context().getOutputPath(outputName).c_str(), std::fstream::binary);
    page.write(output, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page.dataSize(), 0);
    ISHIKO_TEST_FAIL_IF_NEQ(page.availableSpace(), 4080);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(outputName);
    ISHIKO_TEST_PASS();
}

void PageTests::GetTest1(Test& test)
{
    boost::filesystem::path inputPath = test.context().getDataPath("PageTests_GetTest1.dpdb");

    Page page{0};

    Error error;
    std::ifstream input(inputPath.c_str(), std::fstream::binary);
    page.read(input, error);

    ISHIKO_TEST_ABORT_IF(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page.dataSize(), 6);
    ISHIKO_TEST_FAIL_IF_NEQ(page.availableSpace(), 4074);
       
    char buffer[6];
    page.get(buffer, 0, 6, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_NOT(strncmp(buffer, "value1", 6) == 0);
    ISHIKO_TEST_PASS();
}

void PageTests::InsertTest1(Test& test)
{
    const char* outputName = "PageTests_InsertTest1.dpdb";
    
    Error error;

    Page page{0};
    page.init();
    page.insert("value1", 6, 0, error);

    ISHIKO_TEST_FAIL_IF(error);

    std::ofstream output(test.context().getOutputPath(outputName).c_str(), std::fstream::binary);
    page.write(output, error);
    
    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(outputName);
    ISHIKO_TEST_PASS();
}

void PageTests::InsertTest2(Test& test)
{
    const char* outputName = "PageTests_InsertTest2.dpdb";

    Page page{0};

    Error error;
    std::ifstream input(test.context().getDataPath("PageTests_InsertTest2.dpdb").c_str(), std::fstream::binary);
    page.read(input, error);

    ISHIKO_TEST_ABORT_IF(error);
       
    page.insert("value0", 6, 0, error);
            
    ISHIKO_TEST_FAIL_IF(error);
    
    std::ofstream output(test.context().getOutputPath(outputName).c_str(), std::fstream::binary);
    page.write(output, error);
 
    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(outputName);
    ISHIKO_TEST_PASS();
}

/// Tests an insertion that doesn't fit in the current page.
void PageTests::InsertTest3(Test& test)
{
    Page page{0};

    Error error;
    std::ifstream input(test.context().getDataPath("PageTests_InsertTest3.dpdb").c_str(), std::fstream::binary);
    page.read(input, error);

    ISHIKO_TEST_ABORT_IF(error);

    page.insert("", 4075, 0, error);

    ISHIKO_TEST_FAIL_IF_NOT(error);
    ISHIKO_TEST_PASS();
}

void PageTests::EraseTest1(Test& test)
{
    const char* outputName = "PageTests_EraseTest1.dpdb";

    Page page{0};

    Error error;
    std::ifstream input(test.context().getDataPath("PageTests_EraseTest1.dpdb").c_str(), std::fstream::binary);
    page.read(input, error);

    ISHIKO_TEST_ABORT_IF(error);
    
    page.erase(0, 6, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page.dataSize(), 0);
    ISHIKO_TEST_FAIL_IF_NEQ(page.availableSpace(), 4080);
        
    std::ofstream output(test.context().getOutputPath(outputName).c_str(), std::fstream::binary);
    page.write(output, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(outputName);
    ISHIKO_TEST_PASS();
}

void PageTests::EraseTest2(Test& test)
{
    const char* ouputName = "PageTests_EraseTest2.dpdb";

    Page page{0};

    Error error;
    std::ifstream input(test.context().getDataPath("PageTests_EraseTest2.dpdb").c_str(), std::fstream::binary);
    page.read(input, error);

    ISHIKO_TEST_ABORT_IF(error);
        
    page.erase(5, 1, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page.dataSize(), 5);
    ISHIKO_TEST_FAIL_IF_NEQ(page.availableSpace(), 4075);
                
    std::ofstream output(test.context().getOutputPath(ouputName).c_str(), std::fstream::binary);
    page.write(output, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(ouputName);
    ISHIKO_TEST_PASS();
}

void PageTests::EraseTest3(Test& test)
{
    const char* outputName = "PageTests_EraseTest3.dpdb";

    Page page{0};

    Error error;
    std::ifstream input(test.context().getDataPath("PageTests_EraseTest3.dpdb").c_str(), std::fstream::binary);
    page.read(input, error);

    ISHIKO_TEST_ABORT_IF(error);
    
    page.erase(2, 10, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page.dataSize(), 2);
    ISHIKO_TEST_FAIL_IF_NEQ(page.availableSpace(), 4078);
    
    std::ofstream output(test.context().getOutputPath(outputName).c_str(), std::fstream::binary);
    page.write(output, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(outputName);
    ISHIKO_TEST_PASS();
}

void PageTests::MoveToTest1(Test& test)
{
    const char* outputName = "PageTests_MoveToTest1.dpdb";

    Page page1{0};

    Error error;
    std::ifstream input(test.context().getDataPath("PageTests_MoveToTest1.dpdb").c_str(), std::fstream::binary);
    page1.read(input, error);

    ISHIKO_TEST_ABORT_IF(error);
    
    Page page2{1};
    page2.read(input, error);
    
    page1.moveTo(0, 6, page2, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page1.dataSize(), 0);
    ISHIKO_TEST_FAIL_IF_NEQ(page2.dataSize(), 6);
    
    std::ofstream output(test.context().getOutputPath(outputName).c_str(), std::fstream::binary);
    page1.write(output, error);

    ISHIKO_TEST_FAIL_IF(error);
    
    page2.write(output, error);
    
    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(outputName);
    ISHIKO_TEST_PASS();
}

void PageTests::MoveToTest2(Test& test)
{
    const char* outputName = "PageTests_MoveToTest2.dpdb";

    Page page1{0};

    Error error;
    std::ifstream input(test.context().getDataPath("PageTests_MoveToTest2.dpdb").c_str(), std::fstream::binary);
    page1.read(input, error);

    ISHIKO_TEST_ABORT_IF(error);

    Page page2{1};
    page2.read(input, error);
    
    page1.moveTo(0, 6, page2, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_NEQ(page1.dataSize(), 0);
    ISHIKO_TEST_FAIL_IF_NEQ(page2.dataSize(), 12);
    
    std::ofstream output(test.context().getOutputPath(outputName).c_str(), std::fstream::binary);
    page1.write(output, error);

    ISHIKO_TEST_FAIL_IF(error);
            
    page2.write(output, error);
                
    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(outputName);
    ISHIKO_TEST_PASS();
}
