require "rake/testtask"
require "rake/clean"

AE_EXTEN_DIR = "ext/asyncengine"
AE_EXTEN_NAME = "asyncengine_ext.so"
AE_LIB_SO = "lib/asyncengine/asyncengine_ext.so"

# Rule to build the extension: this says that the extension should be rebuilt
# after any change to the files in the extension dir.
file AE_LIB_SO =>
  Dir.glob("#{AE_EXTEN_DIR}/*{.rb,.c}") do
    Dir.chdir(AE_EXTEN_DIR) do
      ruby "extconf.rb"  # TODO: set ruby version !
      sh "make"
    end
    cp "#{AE_EXTEN_DIR}/#{AE_EXTEN_NAME}", AE_LIB_SO
  end

# Use 'rake clean' and 'rake clobber' to easily delete generated files
CLEAN.include("#{AE_EXTEN_DIR}/*{.o,.log,.so,.a}")
CLEAN.include("#{AE_EXTEN_DIR}/Makefile")
CLEAN.include(AE_LIB_SO)
CLOBBER.include("#{AE_EXTEN_DIR}/libuv/**/*{.o,.a}")


task :default => :compile

desc "Compile"
task :compile => AE_LIB_SO

Rake::TestTask.new do |t|
  t.libs << "test"
end

# Make the :test task depend on the shared object, so it will be built automatically
# before running the tests.
desc "Run tests"
task :test => AE_LIB_SO
