#include "buffer.h"
#include <gtest/gtest.h>

using namespace pubsub;

TEST(BufferTest, ConstructOk) {
	ASSERT_NO_THROW(Buffer buffer(1024));
}

TEST(BufferTest, MoveCopyAssignOk) {
    Buffer buffer(1024);
    const char* data = "Hello, World!";
    memcpy(buffer.Get(), data, strlen(data) + 1);

    Buffer copy(buffer);
    ASSERT_STREQ(reinterpret_cast<char*>(copy.Get()), data);

    Buffer assigned = buffer;
    ASSERT_STREQ(reinterpret_cast<char*>(assigned.Get()), data);

    Buffer copy_assigned;
    copy_assigned = buffer;
    ASSERT_STREQ(reinterpret_cast<char*>(assigned.Get()), data);

    Buffer moved(std::move(buffer));
    ASSERT_STREQ(reinterpret_cast<char*>(moved.Get()), data);

    // Set up buffer again since it got moved
    buffer.Allocate(1024);
    memcpy(buffer.Get(), data, strlen(data) + 1);

    Buffer move_assigned;
    move_assigned = std::move(buffer);
    ASSERT_STREQ(reinterpret_cast<char*>(move_assigned.Get()), data);
}

TEST(BufferTest, ReallocateOk) {
	Buffer buffer(1024);
	for (int i = 0; i < 1024; ++i)
		buffer.Get()[i] = i;

	ASSERT_NO_THROW(buffer.Allocate(2048));
	ASSERT_EQ(2048, buffer.Size());
	ASSERT_TRUE(buffer);
}

TEST(BufferTest, BufferPtrOk) {
	Buffer buffer(1024);
	BufferPtr buffer_ptr = buffer;
	ASSERT_EQ(buffer_ptr.size, 1024);
}

TEST(BufferTest, FreeOk) {
	Buffer buffer(1024);
	ASSERT_NO_THROW(buffer.Free());
	ASSERT_EQ(0, buffer.Size());
	ASSERT_EQ(nullptr, buffer.Get());
	ASSERT_FALSE(buffer);
}

TEST(BufferTest, AllocateAndInitOk) {
	Buffer buffer(1024);
	buffer.Allocate(2048, 'x');
	for (int i = 0; i < 2048; ++i) 
		ASSERT_EQ(buffer.Get()[i], 'x');
}
