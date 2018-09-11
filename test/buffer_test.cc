#include "buffer.h"
#include <gtest/gtest.h>

using namespace pubsub;

TEST(BufferTest, ConstructOk) {
	ASSERT_NO_THROW(Buffer buffer(1024));
}
