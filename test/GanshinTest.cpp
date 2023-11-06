extern "C"
{
#include "GFSFctional.h"
#include "FileOper.h"
}
#include "gtest/gtest.h"

// 初始化测试
TEST(GFS, init)
{
    void *ret = GFS_init((fuse_conn_info *)NULL, (fuse_config *)NULL);
    ASSERT_EQ(NULL, ret);
}

TEST(GFS, getattr)
{
    struct stat *st = (struct stat *)malloc(sizeof(struct stat));
    // 测试运行
    int ret = GFS_getattr("/", st, (struct fuse_file_info *)NULL);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(st->st_nlink, 1);
    ASSERT_EQ(st->st_mode, 0755);
    free(st);
}

TEST(FILE, divideFile)
{
    // 文件名和扩展名
    char *fname = (char *)malloc(MAX_FILENAME * sizeof(char));
    char *fext = (char *)malloc(MAX_EXTENSION * sizeof(char));
    int ret = divideFileNameByPath("/etc/tess/ok.jpg", fname, fext, GFILE);
    ASSERT_EQ(ret, 0);
    ASSERT_STRCASEEQ(fname, "ok");
    ASSERT_STRCASEEQ(fext, "jpg");

    free(fname);
    free(fext);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}