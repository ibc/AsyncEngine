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
  @_mutex_destroy = Mutex.new

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
        block.call
      else
        call_from_other_thread(block)
      end
      return true
    end

    @_mutex_run.synchronize do
      # TODO: testing
      ensure_no_handles()

      @_handles = {}
      @_blocks = {}
      @_next_ticks = []
      @_thread = Thread.current
      @_running = true

      uv_terminated_by_itself = false
      begin
        init()
        #add_timer(0, block)  # NOTE: EM does it, do I need?
        block.call
        run_uv()
        uv_terminated_by_itself = true
        puts "INFO: AE.run_uv() terminates correctly"
        true
      ensure
        if uv_terminated_by_itself
          # Even if uv_run() terminated by itself, non active handles (i.e. stopped timers) could
          # remain in @_handles, so destroy them.
          destroy_ae_handles()
          run_uv_once()
        else
          puts "WARN: AE.run_uv() terminates abruptly, running AE.destroy_ae_handles() and AE.run_uv_once()..."
          destroy_ae_handles()
          run_uv_once()
        end
        @_thread = nil
        @_running = false
      end
    end  # @_mutex_run.synchronize
  end

  def self.running?
    !!@_running
  end

  def self.stop
    return false  unless running?

    if running_thread?
      destroy_ae_handles()
    else
      call_from_other_thread { destroy_ae_handles() }
    end
    return true
  end

  def self.destroy_ae_handles
    @_mutex_destroy.synchronize do
      puts "NOTICE: AE.destroy_ae_handles() starts..."

      # TODO: needed?
      Thread.exclusive do
        puts "NOTICE: AE.destroy_ae_handles(): @_handles:#{@_handles.size}, @_blocks:#{@_blocks.size}, @_next_ticks:#{@_next_ticks.size}"
        @_handles.each_value { |handle| handle.send :destroy }
        @_handles.clear
        @_blocks.clear
        @_next_ticks.clear
      end

      puts "NOTICE: AE.destroy_ae_handles() terminates"
    end  # @_mutex_destroy.synchronize
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
      puts "AE.handle_exception(e = #{e.class}: #{e.inspect})"
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
    puts "- Num @_handles: #{@_handles.size}"
    puts "- @_handles:\n"
      @_handles.each {|h| puts "  - #{h.inspect}"}
    puts "- @_blocks:\n"
      @_blocks.each {|b| puts "  - #{b.inspect}"}
    puts "- @_blocks:\n"
      @_next_ticks.each {|n| puts "  - #{n.inspect}"}
    puts
  end

  class << self
    private :run_uv
    private :run_uv_once
    private :destroy_ae_handles
    private :handle_exception
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine