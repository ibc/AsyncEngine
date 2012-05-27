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


module AsyncEngine

  @_pid = Process.pid
  @_exception_handler = nil
  @_mutex_run = Mutex.new
  @_mutex_run = Mutex.new

  def self.run pr=nil, &bl
    raise ArgumentError, "no block given"  unless (block = pr || bl)
    raise ArgumentError, "not a block or Proc"  unless block.is_a? Proc

    # NOTE: For now avoid forking.
    raise AsyncEngine::Error, "cannot run AsyncEngine from a forked process"  unless Process.pid == @_pid

    @_handles ||= {}
    @_blocks ||= {}
    @_next_ticks ||= []

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
        # run_uv() can exit due:
        # - AE.stop, which also called to release() so there should be no more handles.
        # - UV has no *active* handles (but it could have inactive handles, i.e. stopped
        #   timers, so release() function will close them).
        # - An exception/interrupt occurs (run_uv() does not exit in fact) so the
        #   ensure block will close the existing handles.
        release()  if @_running
        released = true
      ensure
        release()  unless released
        ensure_no_handles()  # TODO: for testing
      end
    end  # @_mutex_run.synchronize
  end

  def self.running?
    !!@_running
  end

  def self.release
    # TODO: needed?
    Thread.exclusive do
      # AE handle's destroy() removes the AE handle itself from @_handles.
      @_handles.each_value { |handle| handle.send :destroy }
      @_blocks.clear
      @_next_ticks.clear
      run_uv_once()
      @_thread = nil
      @_running = false
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
    if @_exception_handler
      @_exception_handler.call e
    else
      # TODO: debug
      #puts "WARN: AE.handle_exception(e = #{e.class}: #{e})"
      raise e
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
    private :run_uv_once
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