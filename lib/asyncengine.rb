# AsyncEngine native C extension.
require "asyncengine/asyncengine_ext.so"

# AsyncEngine Ruby library files.
require "asyncengine/version.rb"
require "asyncengine/errors.rb"
require "asyncengine/async.rb"
require "asyncengine/timer.rb"
require "asyncengine/next_tick.rb"
require "asyncengine/udp.rb"


module AsyncEngine

  @_pid = Process.pid
  @_exception_handler = nil
  @_mutex_run = Mutex.new
  @_mutex_run = Mutex.new

  # TMP: No hará falta, solo para AE.debug.
  @_handles = {}
  @_blocks = {}
  @_next_ticks = []

  def self.run pr=nil, &bl
    raise ArgumentError, "no block given"  unless (block = pr || bl)
    raise ArgumentError, "not a block or Proc"  unless block.is_a? Proc

    # TODO: Not true: https://github.com/joyent/libuv/issues/423
    raise AsyncEngine::Error, "cannot run AsyncEngine from a forked process"  unless Process.pid == @_pid

    if running?
      if running_thread?
        puts "NOTICE: AE.run() called while AsyncEngine already running => block.call"
        block.call
      else
        puts "NOTICE: AE.run() called while AsyncEngine already running => call_from_other_thread(block)"
        call_from_other_thread(block)
      end
      return true
    else
      unless clean?
        puts "WARN: AE.run(): not running but clean? => false !!!"  # TODO
        raise AsyncEngine::Error, "AsyncEngine not running but not clean, wait a bit"
      end
    end

    @_mutex_run.synchronize do
      ensure_no_handles()  # TODO: testing

      @_handles = {}
      @_blocks = {}
      @_next_ticks = []
      @_thread = Thread.current
      @_running = true

      released = false
      begin
        init()
        #add_timer(0, block)  # NOTE: EM does it, do I need?
        # NOTE: Yes, otherwise I "enable" AE handlers before running uv_run(), is that what I want? Maybe a next_tick? Not needed IMHO.
        #block.call
        next_tick(block)
        run_uv()
      ensure
        # We must prevent an interrupt while in the ensure block. So let's add another ensure.
        begin
          # Even if uv_run() terminated by itself, non active handles (i.e. stopped timers) could
          # remain in @_handles, so destroy them.
          release()
          released = true
        ensure
          release()  unless released
          ensure_no_handles()  # TODO: testing
        end
      end
    end  # @_mutex_run.synchronize
  end

  def self.running?
    !!@_running
  end

  def self.release
    destroy_ae_handles()
    run_uv_once()
    @_thread = nil
    @_running = false
  end

  def self.clean?
    return false  if @_handles.any? or @_blocks.any? or @_next_ticks.any?
    true
  end

  def self.stop
    return false  unless running?

    if running_thread?
      #destroy_ae_handles()  if running?
      stop_uv()  if running?
    else
      return false  unless running?
      call_from_other_thread do
        #destroy_ae_handles()  if running?
        stop_uv()  if running?
      end
    end
    true
  end

  def self.destroy_ae_handles
    #puts "NOTICE: AE.destroy_ae_handles() starts..."

    # TODO: needed?
    Thread.exclusive do
      #puts "NOTICE: AE.destroy_ae_handles(): @_handles:#{@_handles.size}, @_blocks:#{@_blocks.size}, @_next_ticks:#{@_next_ticks.size}"
      @_handles.each_value { |handle| handle.send :destroy }
      #@_handles.clear  # NOTE: #destroy removes itself from @_handles so no need for it.
      @_blocks.clear
      @_next_ticks.clear
    end

    #puts "NOTICE: AE.destroy_ae_handles() terminates"
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
      # TODO: dbg
      puts "WARN: AE.handle_exception(e = #{e.class}: #{e})"
      raise e
    end
  end

  # TODO: testing
  def self.ensure_no_handles
    raise AsyncEngine::Error, "@_handles not empty"  unless @_handles.empty?
    raise AsyncEngine::Error, "@_blocks not empty"  unless @_blocks.empty?
    raise AsyncEngine::Error, "@_next_ticks not empty"  unless @_next_ticks.empty?
  end

  # TODO: testing
  def self.debug
    puts "\nDBG: AsyncEngine debug:"
    puts "- AE.running: #{running?}"
    puts "- UV active handles: #{num_uv_active_handles()}"
    puts "- @_handles (#{@_handles.size}):\n"
      @_handles.to_a[0..10].each {|k,v| puts "  - #{k}: #{v.inspect}"}
    puts "- @_blocks (#{@_blocks.size}):\n"
      @_blocks.to_a[0..10].each {|k,v| puts "  - #{k}: #{v.inspect}"}
    puts "- @_next_ticks (#{@_next_ticks.size}):\n"
      @_next_ticks[0..10].each {|n| puts "  - #{n.inspect}"}
    puts
  end

  class << self
    private :run_uv
    private :run_uv_once
    private :stop_uv
    private :num_uv_active_handles
    private :release
    private :clean?
    private :destroy_ae_handles
    private :handle_exception
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine