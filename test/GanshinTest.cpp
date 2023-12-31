extern "C"
{
#include "GFSFctional.h"
#include "FileOper.h"
}
#include "gtest/gtest.h"

// 初始化测试功能
TEST(GFS, init)
{
    void *ret = GFS_init((fuse_conn_info *)NULL, (fuse_config *)NULL);
    ASSERT_EQ(NULL, ret);
}
// 测试读取文件和目录信息功能
TEST(GFS, getattr)
{
    struct stat *st = (struct stat *)malloc(sizeof(struct stat));
    // 测试运行
    int ret = GFS_getattr("/", st, (struct fuse_file_info *)NULL);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(st->st_nlink, 1);
    ASSERT_EQ(st->st_mode, __S_IFDIR | 0755);
    free(st);
}
// 测试分离文件名函数
TEST(FILE, divideFile)
{
    // 文件名和扩展名
    char *fname = (char *)malloc((MAX_FILENAME + 1) * sizeof(char));
    char *fext = (char *)malloc((MAX_EXTENSION + 1) * sizeof(char));
    char *remain_path = (char *)malloc((MAX_PATH_LENGTH + 1) * sizeof(char));
    char *fall_name = (char *)malloc((MAX_EXTENSION + MAX_FILENAME + 2) * sizeof(char));
    int ret = divideFileNameByPath("/etc/tess/ok.jpg", fname, fext, fall_name, remain_path, GFILE);
    ASSERT_EQ(ret, 0);
    ASSERT_STRCASEEQ(fname, "ok");
    ASSERT_STRCASEEQ(fext, "jpg");
    ASSERT_STRCASEEQ(remain_path, "/etc/tess");
    ret = divideFileNameByPath("/etc/enaf/hadad/", fname, fext, fall_name, remain_path, GDIRECTORY);
    ASSERT_EQ(ret, 0);
    ASSERT_STRCASEEQ(fname, "hadad");
    ASSERT_STRCASEEQ(fext, "");
    ASSERT_STRCASEEQ(remain_path, "/etc/enaf");

    free(fname);
    free(fext);
    free(remain_path);
    free(fall_name);
}
// 测试新建文件函数
TEST(FILE, createFile)
{
    int ret = createFileByPath("/test.jpg", GFILE);
    ASSERT_EQ(ret, 0);
    struct GFileDir *fd = (struct GFileDir *)malloc(sizeof(struct GFileDir));
    ret = getFileDirByPath("/test.jpg", fd);
    ASSERT_STRCASEEQ(fd->fname, "test");
    ASSERT_STRCASEEQ(fd->fext, "jpg");
    // ASSERT_EQ(fd->fsize, 0);
    free(fd);
}
// 测试读写文件函数
TEST(FILE, writeAndread)
{
    char *data = (char *)malloc(MAX_DATA_IN_BLOCK);
    initShortIntToData(data);
    short int read_int = retShortIntFromData(data, 5);
    ASSERT_EQ(read_int, -1);
    int ret = writeShortIntToData(98, 5, data);
    ASSERT_EQ(ret, 0);
    read_int = retShortIntFromData(data, 5);
    ASSERT_EQ(read_int, 98);
}
// 测试删除文件函数
TEST(FILE, removeFile)
{
    int ret = removeFileByPath("/test.jpg", GFILE);
    ASSERT_EQ(ret, 0);

    struct GFileDir *fd = (struct GFileDir *)malloc(sizeof(struct GFileDir));
    ret = getFileDirByPath("/test.jpg", fd);
    ASSERT_EQ(ret, -1);
}
// 测试GFS的写文件功能
TEST(GFS, writeFile)
{
    // int ret = removeFileByPath("/hello.txt", GFILE);
    // ASSERT_EQ(ret, 0);
    // 创建文件
    int ret = createFileByPath("/hello.txt", GFILE);
    ASSERT_EQ(ret, 0);

    const char *test_data = "Hello, GanshinFuse!";

    // 写入文件
    ret = GFS_write("/hello.txt", test_data, strlen(test_data) + 1, 0, (struct fuse_file_info *)NULL);
    ASSERT_NE(ret, 0);
}
// 测试读文件功能
TEST(GFS, readFile)
{
    const char *test_data = "Hello, GanshinFuse!";
    char *read_data = (char *)malloc(strlen(test_data) + 1);
    // 读取文件
    int ret = GFS_read("/hello.txt", read_data, strlen(test_data) + 1, 0, (struct fuse_file_info *)NULL);
    ASSERT_NE(ret, 0);
    ASSERT_STRCASEEQ(read_data, test_data);
    free(read_data);
}
// 测试创建目录功能
TEST(GFS, createDir)
{
    int ret = GFS_mkdir("/test_dir/", (mode_t)NULL);
    ASSERT_EQ(ret, 0);
    struct GFileDir *fd = (struct GFileDir *)malloc(sizeof(struct GFileDir));
    ret = getFileDirByPath("/test_dir/", fd);
    ASSERT_STRCASEEQ(fd->fname, "test_dir");
    ASSERT_STRCASEEQ(fd->fext, "");
    // ASSERT_EQ(fd->fsize, 0);
    free(fd);
}
// 测试删除目录功能
TEST(GFS, removeDir)
{
    int ret = GFS_rmdir("/test_dir/");
    ASSERT_EQ(ret, 0);

    struct GFileDir *fd = (struct GFileDir *)malloc(sizeof(struct GFileDir));
    ret = getFileDirByPath("/test_dir/", fd);
    ASSERT_EQ(ret, -1);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}