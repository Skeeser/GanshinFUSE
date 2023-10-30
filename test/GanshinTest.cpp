extern "C"
{
#include "GFSFctional.h"
}
#include "gtest/gtest.h"

TEST(GFS_init, init)
{
    void *ret = GFS_init((fuse_conn_info *)NULL, (fuse_config *)NULL);
    ASSERT_EQ(NULL, ret);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}