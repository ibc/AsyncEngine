unless RUBY_VERSION >= "1.9.2"
  raise LoadError, "AsyncEngine requires Ruby version >= 1.9.2 (current version is #{RUBY_VERSION})"
end


# AsyncEngine native C extension.
require "asyncengine/asyncengine_ext.so"

# AsyncEngine Ruby library files.
require "asyncengine/version.rb"
require "asyncengine/errors.rb"
require "asyncengine/call_from_other_thread.rb"
require "asyncengine/next_tick.rb"
require "asyncengine/handle.rb"
require "asyncengine/timer.rb"
require "asyncengine/udp.rb"
require "asyncengine/tcp.rb"
require "asyncengine/utils.rb"
require "asyncengine/debug.rb"  # TODO: Temporal.


module AsyncEngine

  @_pid = Process.pid
  @_exception_handler = nil
  @_handles = {}
  @_blocks = {}
  @_next_ticks = []
  @_call_from_other_thread_procs = []
  @_exit_error = nil

  def self.stop
    return false  unless check_status()

    if running_thread?
      next_tick { release_loop() }
    else
      call_from_other_thread { release_loop() }
    end
    true
  end

  def self.set_exception_handler pr=nil, &bl
    block = pr || bl
    raise ArgumentError, "no block given"  unless block.is_a? Proc

    @_exception_handler = block
  end

  def self.unset_exception_handler
    @_exception_handler = nil
  end

  def self.handle_exception e
    #puts "RB_DBG: AE.handle_exception() called with e = #{e.class}, #{e.message}"
    if @_exception_handler and e.is_a? StandardError
      begin
        @_exception_handler.call e
      rescue Exception => e2
        puts "ERROR: exception #{e2.inspect} during AE.handle_exception(#{e.inspect}) (with exception_handler defined by user)"  # TODO: debug
        @_exit_error = e2
        release_loop()
      end
    else
      # TODO: sometimes I get (e = Fixnum: 8) again: https://github.com/ibc/AsyncEngine/issues/4
      #puts "WARN: AE.handle_exception(#{e.inspect}) (no exception_handler defined by user)"
      @_exit_error = e
      release_loop()
    end
  end

  class << self
    private :release_loop
    private :check_status
    private :handle_exception
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine
