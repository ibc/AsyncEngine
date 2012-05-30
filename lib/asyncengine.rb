unless RUBY_VERSION >= "1.9.2"
  raise LoadError, "AsyncEngine requires Ruby version >= 1.9.2 (current version is #{RUBY_VERSION})"
end


# AsyncEngine native C extension.
require "asyncengine/asyncengine_ext.so"

# AsyncEngine Ruby library files.
require "asyncengine/version.rb"
require "asyncengine/errors.rb"
require "asyncengine/async.rb"
require "asyncengine/next_tick.rb"
require "asyncengine/handle.rb"
require "asyncengine/timer.rb"
require "asyncengine/udp.rb"
require "asyncengine/tcp.rb"
require "asyncengine/utils.rb"


module AsyncEngine

  @_pid = Process.pid
  @_exception_handler = nil
  @_mutex_run = Mutex.new

  def self.run pr=nil, &bl
    raise ArgumentError, "no block given"  unless (block = pr || bl)
    raise ArgumentError, "not a block or Proc"  unless block.is_a? Proc

    # NOTE: For now avoid forking.
    raise AsyncEngine::Error, "cannot run AsyncEngine from a forked process"  unless Process.pid == @_pid

    @_handles ||= {}
    @_blocks ||= {}
    @_next_ticks ||= []
    @_exit_exception = nil

    init();

    if @_running
      if running_thread?
        #puts "NOTICE: AE.run() called while AsyncEngine already running => next_tick(block)"
        next_tick(block)
      else
        #puts "NOTICE: AE.run() called while AsyncEngine already running => call_from_other_thread(block)"
        call_from_other_thread(block)
      end
      # Return nil, which tells the user that the block has been integrated in the
      # already existing AsyncEngine reactor.
      return nil
    else
      unless clean?
        #release()  # TODO: Shuldn't but I've seen a case. Maybe let it? (be polite).
        puts "ERROR: AsyncEngine not running but not clean"
        #raise AsyncEngine::Error, "AsyncEngine not running but not clean, wait a bit"
      end
    end

    # TODO: debug
    if @_mutex_run.locked?
      puts "NOTICE: AE.run(): @_mutex_run is locked"
    end

    @_mutex_run.synchronize do
      ensure_no_handles()  # TODO: for testing

      released = false
      begin
        @_thread = Thread.current
        @_running = true
        pre_run()
        next_tick(block)
        run_uv()
        #puts "DBG: AE.run_uv() exits ok" # TODO
        # run_uv() can exit due:
        # - UV has no *active* handles so it exits (but it could have inactive handles, i.e. stopped
        #   timers, so release() function will close them since @_running is still true).
        # - AE.stop, which also called to release() so @_running is now false.
        release()  if @_running
        #puts "DBG: 'AE.release() if @_running' executed"  if @_running  # TODO
        released = true
      ensure
        release()  unless released
        ensure_no_handles()  # TODO: for testing
        if @_exit_exception
          #puts "WARN: AE.run: there is @_exit_exception (#{@_exit_exception.inspect}), raise it !!!"  # TODO
          e, @_exit_exception = @_exit_exception, nil
          raise e
        end
      end
    end  # @_mutex_run.synchronize
  end

  def self.running?
    !!@_running
  end

  def self.release
    Thread.exclusive do
      # First clear @_next_ticks since UV idle is the first to run when calling to run_uv_release().
      @_next_ticks.clear
      # Then run #destroy() in every AE handle. It will close the UV handle but will not
      # remote the AE handle from @_handles. This is required since some UV reqs could happen
      # later during run_uv_release() and would try to access to the AE handle that should not be
      # GC'd yet.
      begin
        @_handles.each_value { |handle| handle.send :destroy }
      rescue Exception => e
        @_handles.each_value { |handle| handle.send :destroy }
        @_exit_exception ||= e
      end
      # Call to run_uv_release() so UV can execute uv_close callbacks and reqs callbacks.
      run_uv_release()
      begin
        @_blocks.clear
        @_thread = nil
        @_running = false
      rescue Exception => e
        @_blocks.clear
        @_thread = nil
        @_running = false
        @_exit_exception ||= e
      end
    end
  end

  def self.clean?
    return false  if @_handles.any? or @_blocks.any? or @_next_ticks.any?
    true
  end

  def self.stop
    return false  unless ready_for_handles?()

    if running_thread?
      stop_uv()
      release()
    else
      call_from_other_thread do
        stop_uv()
        release()
      end if ready_for_handles?()
    end
    true
  end

  def self.running_thread?
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
    if @_exception_handler and e.is_a? StandardError
      begin
        @_exception_handler.call e
      rescue Exception => e2
        puts "ERROR: exception #{e2.inspect} during AE.handle_exception(#{e.inspect}) (with exception_handler defined by user)"  # TODO: debug
        @_exit_exception = e2
        release()
      end
    else
      # TODO: sometimes I get (e = Fixnum: 8) again: https://github.com/ibc/AsyncEngine/issues/4
      #puts "WARN: AE.handle_exception(#{e.inspect}) (no exception_handler defined by user)"
      @_exit_exception = e
      release()
    end
  end

  # TODO: for testing
  def self.ensure_no_handles
    raise AsyncEngine::Error, "num_uv_active_handles = #{num_uv_active_handles()} (> 1)"  unless num_uv_active_handles() <= 1
    raise AsyncEngine::Error, "@_handles not empty"  unless @_handles.empty?
    raise AsyncEngine::Error, "@_blocks not empty"  unless @_blocks.empty?
    raise AsyncEngine::Error, "@_next_ticks not empty"  unless @_next_ticks.empty?
  end

  # TODO: for testing
  def self.debug
    puts "\nDBG: AsyncEngine debug:"
    puts "- AE.running: #{running?}"
    puts "- UV active handles: #{num_uv_active_handles()}"
    puts "- UV active reqs: #{num_uv_active_reqs()}"
    puts "- @_handles (#{(@_handles ||= {}).size}):\n"
      @_handles.to_a[0..10].each {|k,v| puts "  - #{k}: #{v.inspect}"}
    puts "- @_blocks (#{(@_blocks ||= {}).size}):\n"
      @_blocks.to_a[0..10].each {|k,v| puts "  - #{k}: #{v.inspect}"}
    puts "- @_next_ticks (#{(@_next_ticks ||= []).size}):\n"
      @_next_ticks[0..10].each {|n| puts "  - #{n.inspect}"}
    puts
  end

  class << self
    private :init
    private :pre_run
    private :run_uv
    private :run_uv_release
    private :stop_uv
    private :ready_for_handles?
    private :ensure_ready_for_handles
    private :num_uv_active_handles
    private :release
    private :clean?
    private :handle_exception
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine


# Declare Utils module.
module AsyncEngine::Utils ; end