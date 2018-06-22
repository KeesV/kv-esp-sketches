Import("env", "projenv")
import os, shutil
import gzip

# access to global build environment
#print env

# access to project build environment (is used source files in "src" folder)
#print projenv

#
# Dump build environment (for debug purpose)
# print env.Dump()#
#

#
# Upload actions
#

def empty_folder(folder):
    for the_file in os.listdir(folder):
        file_path = os.path.join(folder, the_file)
        try:
            if os.path.isfile(file_path):
                os.unlink(file_path)
            elif os.path.isdir(file_path): shutil.rmtree(file_path)
        except Exception as e:
            print(e)

def gzip_file(file, destination):
    with open(file, 'rb') as f_in:
        with gzip.open(destination, 'wb') as f_out:
            shutil.copyfileobj(f_in, f_out)

def before_buildfs(source, target, env):
    print "Creating compressed files for SPIFFS"
    # do some actions
    empty_folder("./data")
    shutil.copyfile("./data_uncompressed/index.html", "./data/index.html")
    os.makedirs("./data/bootstrap/css")
    gzip_file("./data_uncompressed/bootstrap/css/bootstrap.min.css", "./data/bootstrap/css/bootstrap.min.gz")

    # call Node.JS or other script
    # env.Execute("node --version")


print "Current build targets", map(str, BUILD_TARGETS)

# custom action before building SPIFFS image. For example, compress HTML, etc.
env.AddPreAction("$BUILD_DIR/spiffs.bin", before_buildfs)
