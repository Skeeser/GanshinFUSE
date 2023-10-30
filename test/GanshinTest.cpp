extern "C"
{
#include "GFSFctional.h"
}
#include "gtest/gtest.h"

TEST(GFS_init, init)
{
    ASSERT_EQ(NULL, GFS_init((fuse_conn_info *)NULL, (fuse_config *)NULL));
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}