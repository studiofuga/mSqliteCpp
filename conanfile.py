from conans import ConanFile, CMake, tools


class MsqlitecppConan(ConanFile):
    name = "msqlitecpp"
    version = "1.99.3"
    license = "BSD3"
    author = "Federico Fuga <fuga@studiofuga.com>"
    url = "https://github.com/studiofuga/mSqliteCpp"
    description = "A modern SQLite C++ interface"
    topics = ("sqlite3", "c++")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": True}
    generators = "cmake_find_package"
    exports_sources = "*"

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".", args=["-DENABLE_TEST=OFF"])
        cmake.build()

        # Explicit way:
        # self.run('cmake %s/hello %s'
        #          % (self.source_folder, cmake.command_line))
        # self.run("cmake --build . %s" % cmake.build_config)

    def build_requirements(self):
        self.build_requires("boost/[1.69.0]@conan/stable")
        self.build_requires("sqlite3/[3.21.0]@bincrafters/stable")

    def package(self):
        self.copy("*.h", dst="include", src="include")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["msqlitecpp"]
