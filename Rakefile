require "rake/testtask"
require "rake/clean"

AE_EXTEN_DIR = "ext/asyncengine"
AE_EXTEN_NAME = "asyncengine_ext.so"
AE_LIB_SO = "lib/asyncengine/asyncengine_ext.so"

# rule to build the extension: this says
# that the extension should be rebuilt
# after any change to the files in ext
file AE_LIB_SO =>
  Dir.glob("#{AE_EXTEN_DIR}/*{.rb,.c}") do
    Dir.chdir(AE_EXTEN_DIR) do
#       Dir.chdir("libuv") do
#         sh "make"
#       end
      # this does essentially the same thing
      # as what RubyGems does
      ruby "extconf.rb"  # TODO: set tuby version !
      sh "make"
    end
    cp "#{AE_EXTEN_DIR}/#{AE_EXTEN_NAME}", AE_LIB_SO
  end

# make the :test task depend on the shared
# object, so it will be built automatically
# before running the tests
task :test => AE_LIB_SO

# use 'rake clean' and 'rake clobber' to
# easily delete generated files
CLEAN.include("#{AE_EXTEN_DIR}/*{.o,.log,.so,.a}")
CLEAN.include("#{AE_EXTEN_DIR}/Makefile")
#CLEAN.include("#{AE_EXTEN_DIR}/libuv/**/*{.o,.a}")
CLOBBER.include("#{AE_EXTEN_DIR}/libuv/**/*{.o,.a}")
CLOBBER.include(AE_LIB_SO)

# the same as before
Rake::TestTask.new do |t|
  t.libs << "test"
end

desc "Run tests"
task :default => :test

desc "Compile"
task :compile