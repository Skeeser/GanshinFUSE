/*
Author: keeser
Email: 1326658417@qq.com
Liense: GPL 2.0
Github: https://github.com/Skeeser/GanshinFUSE
*/
#include "FileOper.h"

// 根据块号, 读取文件数据
int readDataByBlkId(long blk_id,struct DataBlock *data_blk)
{
	// 读取文件
    FILE *fp = NULL;
    fp = fopen(DISK_PATH, "r+");//打开文件
    if(fp == NULL){
        printError("Open disk file failed! The file may don't exits.");
        return 0;
    }
    // printSuccess("Open disk file success!");

	// 文件打开后，就用blk_id * FS_BLOCK_SIZE作为偏移量
	fseek(fp, blk_id * FS_BLOCK_SIZE, SEEK_SET);
	fread(data_blk, sizeof(struct DataBlock), 1 , fp);
	
    // 判断是否正确读取
    if(ferror(fp))
	{
		printError("Read disk file failed!");
        return -1;
	}

	fclose(fp);
    return 0;
}

// 根据路径，到相应的目录寻找文件的GFileDir，并赋值给attr
int readDataByBlkId(const char * path,struct GFileDir *attr)
{
	// 获取磁盘根目录块的位置
	printSuccess("Get File Dir To Attr Start!");
	printf("getFileDirToAttr: get file dir by: %s\n", path);
	
    struct DataBlock *data_blk;
	data_blk = malloc(sizeof(struct DataBlock));

	// 读出超级块
	if (read_cpy_data_block(0, data_blk) == -1) 
	{
		printError("getFileDirToAttr: read super block failed!");
		free(data_blk);	
        return -1;
	}else{
        printSuccess("getFileDirToAttr: read super block success!");
    }


	printf("check2:clear\n\n");
	struct super_block* sb_blk;
	sb_blk = (struct super_block*) data_blk;
	long start_blk;
	start_blk = sb_blk->first_blk;
	printf("检查sb_blk:\nfs_size=%ld\nfirst_blk=%ld\nbitmap=%ld\n\n",sb_blk->fs_size,sb_blk->first_blk,sb_blk->bitmap);
	printf("start_blk:%ld\n\n",start_blk);
	char *tmp_path,*m,*n;//tmp_path用来临时记录路径，然后m,n两个指针是用来定位文件名和
	tmp_path=strdup(path);m=tmp_path;
	printf("check3:clear\n\n");
	//如果路径为空，则出错返回1
	if (!tmp_path) 
	{
		printf("错误：get_fd_to_attr：路径为空，函数结束返回\n\n");
		free(sb_blk);return -1;
	}
	
	//如果路径为根目录路径(注意这里的根目录是指/home/linyueq/homework/diskimg/???，/???之前的路径是被忽略的)
	if (strcmp(tmp_path, "/") == 0) 
	{
		attr->flag = 2;//2代表路径
		attr->nStartBlock = start_blk;
		free(sb_blk);
		printf("get_fd_to_attr：这是一个根目录，直接构造file_directory并返回0，函数结束返回\n\n");
		return 0;
	}
	printf("check4:clear\n\n");
	//检查完字符串既不为空也不为根目录,则要处理一下路径，而路径分为2种（我们只做2级文件系统）
	//一种是文件直接放在根目录下，则路径形为：/a.txt，另一种是在一个目录之下，则路径形为:/hehe/a.txt
	//我们路径处理的目标是让tmp_path记录diskimg下的目录名（如果path含目录的话），m记录文件名，n记录后缀名
	
	//先往后移一位，跳过第一个'/'，然后检查一下这个路径是不是有两个'/'，如果有，说明路径是/hehe/a.txt形
	m++;
	n=strchr(m,'/');
	if(n!=NULL)
	{
		printf("get_fd_to_attr：这是一个二级路径\n\n");
		tmp_path++;//此时tmp_path指着目录名的第一个字母
		*n='\0';//这样设置以后，tmp_path就变成了独立的一个字符串记录		目录名	，到'\0'停止
		n++;
		m=n;//现在m指着		文件名（含后缀名）
	}
	//如果路径不是/hehe/a.txt形，则为/a.txt形，只有一个/，所以上面的n会变成空指针
	//如果路径是/hehe/a.txt形也没有关系，依然能够让p为文件名，q为后缀名
	n=strchr(m,'.');
	if (n!=NULL) 
	{
		printf("get_fd_to_attr：检测到'.'，这是一个文件\n\n");
		*n='\0'; n++;     //q为后缀名
	}
	printf("check5:clear\n\n");
	//struct data_block *data_blk=malloc(sizeof(struct data_block));
	//读取根目录文件的信息，失败的话返回-1
	if (read_cpy_data_block(start_blk, data_blk) == -1) 
	{
		free(data_blk);	printf("错误：get_fd_to_attr：读取根目录文件失败，函数结束返回\n\n");return -1;
	}
	//强制类型转换，读取根目录中文件的信息，根目录块中装的都是struct file_directory
	struct file_directory *file_dir =(struct file_directory*)data_blk->data;
	int offset=0;
	printf("check6:clear\n\n");
	//如果path是/hehe/a.txt形路径（二级路径），那么我们先要进入到该一级目录（根目录下的目录）下
	//如果path是/a.txt形路径（一级路径），那么不会进入该分支，直接
	if (*tmp_path != '/') 
	{	//遍历根目录下所有文件的file_directory，找到该文件
		printf("get_fd_to_attr：二级路径if分支下：这是二级路径，开始寻找其父目录的nstartblock\n\n");
		while (offset < data_blk->size) 
		{
			if (strcmp(file_dir->fname, tmp_path) == 0 && file_dir->flag == 2) //找到该目录
			{
				start_blk = file_dir->nStartBlock; break;    //记录该一级目录文件的起始块
			}
			//没找到目录继续找下一个结构
			file_dir++;
			offset += sizeof(struct file_directory);
		}
	
		//如果最后还是没找到该目录，说明目录不存在，返回-1
		if (offset == data_blk->size)
		{
			free(data_blk);printf("错误：get_fd_to_attr：二级路径if分支下的while循环内：路径错误，根目录下无此目录\n\n");return -1;
		}
		//如果找到了该目录文件的file_directory，则读出一级目录块的文件信息
		if (read_cpy_data_block(start_blk, data_blk) == -1) 
		{
			free(data_blk);printf("错误：get_fd_to_attr：二级路径if分支下：找到了二级路径父目录的nStartBlock，但是打开失败\n\n");return -1;
		}
		file_dir = (struct file_directory*) data_blk->data;
	}
	printf("check7:clear\n\n");
	//重置offset再进行一次file_directory的寻找操作，不过这次是直接寻找path所指文件的file_directory
	offset=0;
	printf("检查file_dir数据：file_dir:\n");
	printf("file_dir:fname=%s，fext=%s，fsize=%ld，nstartblock=%ld，flag=%d\n\n",file_dir->fname,\
	file_dir->fext,file_dir->fsize,file_dir->nStartBlock,file_dir->flag);
	printf("m=%s,n=%s\n\n",m,n);
	while (offset < data_blk->size) 
	{
		printf("check8:根据file_dir循环查找\n\n");
		//对比file_dir中的fname和fext是否与path中一致，借此找到path所指文件的file_directory
		if (file_dir->flag != 0 && strcmp(file_dir->fname, m) == 0 && (n == NULL || strcmp(file_dir->fext, n) == 0 )) 
		{ 
			//进入文件/目录所在块
			start_blk = file_dir->nStartBlock;
			//读出属性
			read_cpy_file_dir(attr, file_dir);//把找到的该文件的file_directory赋值给attr
			free(data_blk);
			printf("get_fd_to_attr：路径所指对象的父目录中匹配到了符合该path的file_directory，函数结束返回\n\n");
			return 0;
		}
		//读下一个文件
		file_dir++;
		offset += sizeof(struct file_directory);
	}
	//循环结束都还没找到，则返回-1
	printf("get_fd_to_attr：在父目录下没有找到该path的file_directory\n\n");
	free(data_blk);
	return -1;
}

/*
//创建path所指的文件或目录的file_directory，并为该文件（目录）申请空闲块，创建成功返回0，创建失败返回-1
//mkdir和mknod这两种操作都要用到
int create_file_dir(const char* path, int flag)
{
	printf("调用了create_file_dir，创建的类型是：%d，创建的路径是：%s\n\n",flag,path);
	int res,par_size;
	long par_dir_blk;//存放父目录文件的起始块，经过exist_check后会指向父目录文件的最后一块（因为创建文件肯定实在最后一块增加file_directory的）
	char *m = malloc(15 * sizeof(char)), *n = malloc(15 * sizeof(char));//用于存放文件名和扩展名
	//拆分路径，找到父级目录起始块
	if ((res = divide_path(m, n, path, &par_dir_blk, flag,&par_size))) 
	{
		free(m);free(n);
		printf("错误：create_file_dir:divide_path时出错\n\n");return res=-1;
	}
	struct data_block *data_blk = malloc(sizeof(struct data_block));
	struct file_directory *file_dir =malloc(sizeof(struct file_directory));
	int offset = 0;
	int pos;
	
	while(1)
	{
		printf("create_file_dir:进入exist_check循环\n\n");
		//从目录块中读取目录信息到data_blk
		if (read_cpy_data_block(par_dir_blk, data_blk) == -1) 
		{
			free(data_blk);free(file_dir);free(m);free(n);
			printf("错误：create_file_dir:从目录块中读取目录信息到data_blk时出错\n\n");return -ENOENT;
		}
		file_dir =(struct file_directory*) data_blk->data;
		offset = 0;
		pos = data_blk->size;
		
		//遍历父目录下的所有文件和目录，如果已存在同名文件或目录，返回-1
		//一个文件一定是连续存放的
		if ((res = exist_check(file_dir, m, n, &offset, &pos, data_blk->size, flag))) 
		{
			free(data_blk);free(file_dir);free(m);free(n);
			printf("错误：create_file_dir:exist_check检测到该文件（目录）已存在，或出错\n\n");return res=-1;
		}
		if(data_blk->nNextBlock==-1||data_blk->nNextBlock==0) break;
		else par_dir_blk=data_blk->nNextBlock;
	}
	printf("create_file_dir:没有重名的文件或目录，开始创建文件\n\n");
	//经过exist_check函数，offset应该指向匹配到的文件的file_dir的位置，如果没找到该文件，则offset=data_blk->size
	file_dir += offset / sizeof(struct file_directory);
	long *tmp = malloc(sizeof(long));
	//假如exist_check函数里面并没有改变pos的值，那么说明flag!=0
	if (pos == data_blk->size) 
	{
		printf("create_file_dir:enlarge_blk开始\n\n");
		//当前块放不下目录内容
		if (data_blk->size > MAX_DATA_IN_BLOCK) 
		{
			printf("create_file_dir:当前块放不下文件，要enlarge_blk\n\n");
			//为父目录文件新增一个块
			if ((res = enlarge_blk(par_dir_blk, file_dir, data_blk, tmp, m, n, flag))) 
			{
				free(data_blk);free(file_dir);free(m);free(n);
				printf("错误：create_file_dir:enlarge_blk出错\n\n");return res;
			}
			free(data_blk);free(file_dir);free(m);free(n);return 0;
		} 
		else 
		{//块容量足够，直接加size
			data_blk->size += sizeof(struct file_directory);
		}
	}
	else
	{//flag=0
		printf("create_file_dir:flag为0\n\n");
		offset = 0;
		file_dir = (struct file_directory*) data_blk->data;
		while (offset < pos) file_dir++;
	}
	//给新建的file_directory赋值
	strcpy(file_dir->fname, m);
	if (flag == 1 && *n!='\0')	strcpy(file_dir->fext, n);
	file_dir->fsize = 0;
	file_dir->flag = flag;
	//找到空闲块作为起始块
	
	//为新建的文件申请一个空闲块
	if ((res = get_empty_blk(1, tmp)) == 1)	file_dir->nStartBlock = *tmp;
	else 
	{
		printf("错误：create_file_dir：为新建文件申请数据块时失败，函数结束返回\n\n");
		free(data_blk);free(file_dir);free(m);free(n);return -errno;
	}
	printf("tmp=%ld\n\n",*tmp);
	free(tmp);
	//将要创建的文件或目录信息写入上层目录中
	write_data_block(par_dir_blk, data_blk);
	data_blk->size = 0;
	data_blk->nNextBlock = -1;
	strcpy(data_blk->data, "\0");
	
	//文件起始块内容为空
	write_data_block(file_dir->nStartBlock, data_blk);
	
	printf("m=%s,n=%s\n\n",m,n);
	free(data_blk);free(m);free(n);
	printf("create_file_dir：创建文件成功，函数结束返回\n\n");
	return 0;
}

//删除path所指的文件或目录的file_directory和文件的数据块，成功返回0，失败返回-1
int remove_file_dir(const char *path, int flag) 
{
	printf("remove_file_dir：函数开始\n\n");
	struct file_directory *attr=malloc(sizeof(struct file_directory));
	//读取文件属性
	if (get_fd_to_attr(path, attr) == -1) 
	{
		free(attr);printf("错误：remove_file_dir：get_fd_to_attr失败，函数结束返回\n\n");	return -ENOENT;
	}
	printf("检查attr:fname=%s，fext=%s，fsize=%ld，nstartblock=%ld，flag=%d\n\n",attr->fname,\
	attr->fext,attr->fsize,attr->nStartBlock,attr->flag);
	//flag与指定的不一致，则返回相应错误信息
	if (flag == 1 && attr->flag == 2)
	{
		free(attr); printf("错误：remove_file_dir：要删除的对象flag不一致，删除失败，函数结束返回\n\n");return -EISDIR;
	} 
	else if (flag == 2 && attr->flag == 1) 
	{
		free(attr); printf("错误：remove_file_dir：要删除的对象flag不一致，删除失败，函数结束返回\n\n");return -ENOTDIR;
	}
	//清空该文件从起始块开始的后续块
	struct data_block* data_blk = malloc(sizeof(struct data_block));
	if (flag == 1) 
	{
		long next_blk = attr->nStartBlock;
		ClearBlocks(next_blk, data_blk);
	} 
	else if (!path_is_emp(path)) //只能删除空的目录，目录非空返回错误信息
	{ 
		free(data_blk);free(attr);
		printf("remove_file_dir：要删除的目录不为空，删除失败，函数结束返回\n\n");
		return -ENOTEMPTY;
	}
	attr->flag = 0; //被删除的文件或目录的file_directory设置为未使用
	if (setattr(path, attr, flag) == -1) 
	{
		printf("remove_file_dir：setattr失败，函数结束返回\n\n");
		free(data_blk);free(attr);return -1;
	}
	printf("remove_file_dir：删除成功，函数结束返回\n\n");
	free(data_blk);free(attr);
	return 0;
}

*/