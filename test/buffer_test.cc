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

