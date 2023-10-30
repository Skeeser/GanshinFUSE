#include "GFSFctional.h"
#include "gtest/gtest.h"


TEST(GFS_init, init){
    ASSERT_EQ(NULL, GFS_init());
}

int main(int argc,char *argv[])
{
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}