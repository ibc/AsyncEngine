unless RUBY_VERSION >= "1.9.2"
  raise LoadError, "AsyncEngine requires Ruby version >= 1.9.2 (current version is #{RUBY_VERSION})"
end


require "mkmf"
require "rbconfig"
require "fileutils"


def sys cmd
  puts "DBG: system command in #{File.expand_path(File.dirname(__FILE__))}:  #{cmd}"
  unless ret = xsystem(cmd)
    abort "system command `#{cmd}' failed, please report to https://github.com/ibc/AsyncEngine/issues"
  end
  ret
end


# TODO: Copied from EventMachine. Is this really good?
def add_define(name)
  $defs.push("-D#{name}")
end


# Ruby checks.
raise "rb_thread_call_with_gvl() not present"  unless have_func("rb_thread_call_with_gvl")
raise "rb_thread_call_without_gvl() not present"  unless have_func("rb_thread_call_without_gvl")


# TODO: Changes needed for Windows, and lot of testing needed here.
case host_os = RbConfig::CONFIG["host_os"]

  when /solaris/i
    cflags = " -G -fPIC "
    RbConfig::CONFIG["LDSHARED"] = "$(CXX) -G -fPIC"
    if RbConfig::CONFIG["CC"] == "cc"
      cflags = "-g -O2 -fPIC"
      RbConfig::CONFIG["CCDLFLAGS"] = "-fPIC"
    end
    ldflags = ""
    add_define "AE_OS_UNIX"

  when /darwin|mac os/i
    cflags = "-shared -fPIC"
    ldflags = "-framework CoreServices"
    add_define "AE_OS_UNIX"

  else
    cflags = "-shared -fPIC"
    ldflags = ""
    add_define "AE_OS_UNIX"
end

$CFLAGS = RbConfig::CONFIG["CFLAGS"] = " #{cflags} "
$LDFLAGS = RbConfig::CONFIG["LDFLAGS"] = " #{ldflags} "

libuv_dir = File.expand_path(File.dirname(__FILE__), "libuv/")

unless File.directory?(libuv_dir)
  sys "./get_libuv.sh"
end

Dir.chdir(libuv_dir) do
  sys "CFLAGS='#{cflags}' make"
  puts "DBG: in #{File.expand_path(File.dirname(__FILE__))}:  FileUtils.mv \"uv.a\", \"../libuv.a\""
  FileUtils.mv "uv.a", "../libuv.a"
end

have_library("uv") or raise "have_library('uv') returns false"

create_makefile("asyncengine/asyncengine_ext")
