require "mkmf"
require "rbconfig"
require "fileutils"


def sys cmd
  puts "system command in #{File.expand_path(File.dirname(__FILE__))}:  #{cmd}"
  unless ret = xsystem(cmd)
    raise "system command `#{cmd}' failed, please report to https://github.com/ibc/AsyncEngine/issues"
  end
  ret
end


case host_os = RbConfig::CONFIG["host_os"]

  when /solaris/i
    cflags = " -G -fPIC "
    RbConfig::CONFIG["LDSHARED"] = "$(CXX) -G -fPIC"
    if RbConfig::CONFIG["CC"] == "cc"
      cflags = "-g -O2 -fPIC"
      RbConfig::CONFIG["CCDLFLAGS"] = "-fPIC"
    end
    ldflags = ""

  when /darwin|mac os/i
    cflags = "-shared -fPIC"
    ldflags = "-framework CoreServices"

  else
    cflags = "-shared -fPIC"
    ldflags = ""
end

$CFLAGS = RbConfig::CONFIG["CFLAGS"] = " #{cflags} "
$LDFLAGS = RbConfig::CONFIG["LDFLAGS"] = " #{ldflags} "

libuv_dir = File.expand_path(File.dirname(__FILE__), "libuv/")

unless File.directory?(libuv_dir)
  sys "./get_libuv.sh"
end

Dir.chdir(libuv_dir) do
  sys "CFLAGS='#{cflags}' make"
  FileUtils.mv "uv.a", "../libuv.a"
end

#dir_config("uv", File.expand_path("../libuv/include", __FILE__), File.expand_path("../libuv", __FILE__))
have_library("uv") or raise "have_library('uv') returns false"

create_makefile("asyncengine/asyncengine_ext")
