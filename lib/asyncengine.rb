# AsyncEngine native C extension.
require "asyncengine/asyncengine_ext.so"

# AsyncEngine Ruby library files.
require "asyncengine/version.rb"
require "asyncengine/errors.rb"
require "asyncengine/timer.rb"
require "asyncengine/next_tick.rb"
require "asyncengine/udp.rb"


module AsyncEngine

  @_exception_handler = nil
  @_blocks = {}
  @_next_ticks = []
  @_handles = {}

  def self.run
    if running?
      puts "RB NOTICE: AE.run() => AsyncEngine is already running, return nil"
      return nil
    end

    # SIGPIPE is received by a program when reading from a pipe whose other
    # end has been closed, so it's good to trap and ignore it:
    #   http://doc.dvgu.ru/devel/ev.html#the_special_problem_of_sigpipe
    #
    # However this is not needed since both UV and Ruby ignore by default
    # SIGPIPE signal.
    trap(:PIPE) {}

    begin
      # Run the block. If AsyncEngine.stop is called here, it will be captured below.
      if block_given?
        @_in_yield = true
        yield
        @_in_yield = nil
      end

      # Run UV.
      @_exit_exception = nil
      if _c_run
        #puts "YEAH: _c_run exits !!!"
        # Any kind of exception (StandardError) is rescued by the exception_handler which
        # sets @_exit_exception, so _c_run always ends with true.
        if @_exit_exception
          _c_run
          puts "NOTICE: exception #{@_exit_exception.class} (#{@_exit_exception}) after _c_run !!! raising it..."
          raise @_exit_exception
        else
          return true
        end
      else
        # TODO: This should NEVER occur!
        abort "ABORT: _c_run does not return true"
      end
    rescue AsyncEngine::StopInYield
      puts "AsyncEngine::StopInYield rescued => true"
      # Run UV once so it executes the async_uv handle and it's removed.
      _c_run
      return true
    rescue Exception => e
      raise e  if @_exit_exception
      puts "NOTICE: exception #{e.class} (#{e}) rescued running _c_run, calling destroy() and raising it"
      destroy
      # Run UV once so it executes the async_uv handle and it's removed.
      _c_run
      raise e # And raise the original exception.
    end
  end

  def self.destroy
    @_handles.each_value { |handle| handle.send :destroy }
    @_handles.clear
    @_blocks.clear
    @_next_ticks.clear
  end

  def self.stop
    destroy
    raise AsyncEngine::StopInYield  if @_in_yield
    true
  end

  def self.set_exception_handler block1=nil, &block2
    block = block1 || block2
    raise AsyncEngine::Error, "no block provided"  unless block.respond_to? :call

    @_exception_handler = block
  end

  def self.unset_exception_handler
    @_exception_handler = nil
  end

  def self.handle_exception e
    if @_exception_handler
      # Protect the code if the user generates an exception in the exception_handler block.
      begin
        @_exception_handler.call e
      rescue => e
        @_exit_exception = e
        destroy
      end
    else
      # Instead of raising an exception (which would bypass uv_run() without proper exiting,
      # store the exception in @_exit_exception and call destroy(), so _c_run() terminates.
      @_exit_exception = e
      destroy
    end
  end

  # TODO: testing
  def self.debug
    puts "\nDBG: AsyncEngine debug:"
    puts "- AE.running?: #{running?.inspect}"
    puts "- AE.num_handles: #{num_handles}"
    puts "- @_blocks: #{@_blocks.inspect}"
    puts "- @_handles: #{@_handles.inspect}"
    puts
  end

  class << self
    private :_c_run
    private :destroy
    private :handle_exception
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine