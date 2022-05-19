from conans import ConanFile, CMake
import os


# channel = os.getenv("CONAN_CHANNEL", "stable")
# username = os.getenv("CONAN_USERNAME", "memsharded")


class fpgenTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "fpgen/1.0.1@demo/testing"
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        # self.run('cmake "%s" %s' % (self.conanfile_directory, cmake.command_line))
        # self.run("cmake --build . %s" % cmake.build_config)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", "bin", "bin")
        self.copy("*.dylib", "bin", "bin")

    def test(self):
        os.chdir("bin")
        self.run(".%sexample" % os.sep)
