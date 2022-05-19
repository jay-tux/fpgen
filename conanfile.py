from conans import ConanFile


class Fpgen(ConanFile):
    name = "fpgen"
    version = "1.0.1"
    # header only, so no settings/options
    exports_sources = "inc/*", "include/*"
    no_copy_source = True

    def package_info(self):
        self.cpp_info.includedirs = ["inc", "include"]

    def package(self):
        self.copy("*.hpp", dst="include", src="inc")
        self.copy("*.h", dst="include", src="inc")

    def package_id(self):
        self.info.header_only()
