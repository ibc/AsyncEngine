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


module AsyncEngine

  @_pid = Process.pid
  @_exception_handler = nil
  @_handles = {}
  @_blocks = {}
  @_next_ticks = []
  @_call_from_other_thread_procs = []
  @_exit_exception = nil
  @_mutex_run = Mutex.new

  def self.run pr=nil, &bl

    # TODO: Ya no puedo usar un mutex porque ahora fallaria en AE.run en AE.run.
    #@_mutex_run.synchronize do
      run_loop(pr || bl)

      if @_exit_exception
        puts "WARN: AE.run: there is @_exit_exception (#{@_exit_exception.inspect})"  # TODO
        e, @_exit_exception = @_exit_exception, nil
        # TODO: https://github.com/ibc/AsyncEngine/issues/4
        # De momento comprobamos que sea Exception...
        raise e  if e.is_a? Exception
      end
    #end  # @_mutex_run.synchronize

    ensure_no_handles()  unless AE.running? # TODO: for testing

    return true
  end

  def self.stop
    return false  unless running?()

    if running_thread?
      #next_tick do
        release_loop()
      #end
    else
      call_from_other_thread do
        release_loop()
      end if running?()
    end
    true
  end

  def self._running_thread?
    Thread.current == @_thread
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
        @_exit_exception = e2
        release_loop()
      end
    else
      # TODO: sometimes I get (e = Fixnum: 8) again: https://github.com/ibc/AsyncEngine/issues/4
      #puts "WARN: AE.handle_exception(#{e.inspect}) (no exception_handler defined by user)"
      @_exit_exception = e
      release_loop()
    end
  end

  # TODO: for testing
  def self.ensure_no_handles
    raise AsyncEngine::Error, "num_uv_active_handles = #{num_uv_active_handles()} (> 1)"  unless num_uv_active_handles() <= 1
    raise AsyncEngine::Error, "@_handles not empty"  unless @_handles.empty?
    raise AsyncEngine::Error, "@_blocks not empty"  unless @_blocks.empty?
    raise AsyncEngine::Error, "@_next_ticks not empty"  unless @_next_ticks.empty?
    raise AsyncEngine::Error, "@_call_from_other_thread_procs not empty"  unless @_call_from_other_thread_procs.empty?
    @_call_from_other_thread_procs
  end

  # TODO: for testing
  def self.debug
    puts "\nDBG: AsyncEngine debug:"
    puts "- AE.running: #{running?}"
    puts "- UV active handles: #{num_uv_active_handles()}"
    puts "- UV active reqs: #{num_uv_active_reqs()}"
    puts "- @_handles (#{@_handles.size}):\n"
      @_handles.to_a[0..10].each {|k,v| puts "  - #{k}: #{v.inspect}"}
    puts "- @_blocks (#{@_blocks.size}):\n"
      @_blocks.to_a[0..10].each {|k,v| puts "  - #{k}: #{v.inspect}"}
    puts "- @_next_ticks (#{@_next_ticks.size}):\n"
      @_next_ticks[0..10].each {|n| puts "  - #{n.inspect}"}
    puts "- @_call_from_other_thread_procs (#{@_call_from_other_thread_procs.size}):\n"
      @_call_from_other_thread_procs[0..10].each {|n| puts "  - #{n.inspect}"}
    puts
  end

  class << self
    private :run_loop
    private :release_loop
    private :check_running
    private :num_uv_active_handles
    private :handle_exception
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine
