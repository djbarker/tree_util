/*
 * Simple utility to print the directory structure and the number of files in
 * each directory. Unlike 'tree' it does not list regular files. Uses low level
 * system calls for maximum speed meaning it can handle directories containing
 * many thousands of files and above.
 * 
 * Adapted from: https://github.com/aidenbell/getdents
 */

// C headers
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>

// C++ headers
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <utility>

using namespace std;

struct linux_dirent {
   long d_ino;
   off_t d_off;
   unsigned short d_reclen;
   char d_name[];
};

#define BUF_SIZE 1024*1024 // 1024 kB

void print_dir(string path, string full_path, int indent, vector<bool> last)
{
  char buf[BUF_SIZE];
 
  int fd = open(full_path.c_str(), O_RDONLY | O_DIRECTORY);

  if(fd==-1)
  {
    stringstream msg;
    msg << "Error opening "<< full_path << ".";
    throw runtime_error(msg.str());
  }

  vector<pair<string,string>> sub_dirs;
  int file_count=0;

  while(true)
    {
      int nread = syscall(SYS_getdents,fd,buf,BUF_SIZE);
      if(nread==-1)
      {
	stringstream msg;
	msg << "Error reading open file-descriptor for " << full_path << ".";
	throw runtime_error(msg.str());
      }

      if(nread==0) break;
      for(int bpos=0;bpos<nread;)
	{
	  linux_dirent* d = (linux_dirent*)(buf+bpos);
	  char d_type = *(buf + bpos + d->d_reclen -1);
	  char* name = (char*)d->d_name;
	
	  if( d->d_ino!=0 && d_type==DT_DIR && name[0]!='.') // ignore ., .. and hidden folders
	  {
	    string sd_full_path = full_path + '/' +  string(name);
	    sub_dirs.push_back(make_pair(string(name),sd_full_path));
	  }
	  else if(d->d_ino!=0 && d_type!=DT_DIR) // some files return as DT_UNKNOWN not DT_REG for some reason?
	{
		file_count++;
	}

	  bpos += d->d_reclen;
	}
    }

  	for(int i=0;i<indent-1;++i) 
	{
		cout << (last[i]?"    ":"│   ");
	}
	if(indent>0) cout << (last[indent-1]?"└── ":"├── ");

  	cout << path << "/" << " [";
	
	if(file_count<100)  cout << "\033[0;32m"; // green
	else if(file_count<1000) cout << "\033[0;33m"; // yellow
	else cout << "\033[0;31m"; // red
	
	cout << file_count << " files\033[0m, ";
	
	if(sub_dirs.size()<100)  cout << "\033[0;32m"; // green
	else if(sub_dirs.size()<1000) cout << "\033[0;33m"; // yellow
	else cout << "\033[0;31m"; // red
	
	cout << sub_dirs.size() << " dirs\033[0m]" << endl; 
	
	int tmp = 0;
	for(auto spair: sub_dirs)
	{
		auto cpy = last;
		cpy.push_back(tmp==(sub_dirs.size()-1));
		print_dir(spair.first.c_str(),spair.second.c_str(),indent+1,cpy);
		tmp++;
	}
}

int main(int argc, char *argv[])
{
   vector<bool> last;

   string dir = ".";
   if(argc>1)
	dir = string(argv[1]);

   print_dir(dir,dir,0,last);

   return 0;
}
