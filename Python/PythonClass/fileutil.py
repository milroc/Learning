import os
import fnmatch
import shutil

def glob_all(rootdir, regex):
    ret = []
    for root, dir, files in os.walk(rootdir):
        for file in files:
            if fnmatch.fnmatch(file, regex):
                ret.append(os.path.join(root,file))
    return ret
        
        
def copy_matching_files(srcdir,targetdir,pattern):
    filenames = glob_all(srcdir,pattern)
    for source_name in filenames:
        target_name = source_name.replace(srcdir,targetdir,1)
        target_dir = os.path.dirname(target_name)
        if not os.path.exists(target_dir):
            os.makedirs(target_dir)
        print "Copying: ", source_name
        shutil.copy(source_name, target_name)

if __name__ == '__main__':
    import sys
    if len(sys.argv) != 4:
        print "Usage: %s srcdir targetdir pattern" % sys.argv[0]
        raise SystemExit(1)
    copy_matching_files(sys.argv[1],sys.argv[2],sys.argv[3])
