# AsyncEngine vative C extension.
require "asyncengine/asyncengine_ext.so"

# AsyncEngine Ruby library files.
require "asyncengine/version.rb"
require "asyncengine/errors.rb"
require "asyncengine/timers.rb"


module AsyncEngine

  @handles = {}

  def self.start
    yield  if block_given?

    if self._c_start
      return true
    else
      raise AsyncEngine::Error, "uv_run failed to start"
    end
  end


  # Must be run before AsyncEngine.start (TODO: check it).
  # TODO: Si mando una signal al proceso que no ha sido declarada con on_signal,
  # entonces obviamente no pasa nada, pero cuando mando luego una signal declarada
  # con on_signal entonces resulta que el proceso Ruby actúa como si se hubiese
  # recibido la primera signal.
  # TODO: Esto no va a funcionar creo, ya que cuando esté todo funcionando con callbacks
  # de Ruby, me temo que al enviar una signal puede que sea Ruby el que la capture (no se).
  def self.on_signal signal, &callback
    @trapped_signals ||= {}

    # Normalize the signal name.
    signal_name = signal.to_s.upcase.sub(/^SIG/, "")

    raise "invalid signal name `#{signal}'" unless (signal_number = ::Signal.list[signal_name])
    raise "no callback provided"  unless callback

    if self.trap_signal signal_number
      @trapped_signals[signal_number] = callback
      true
    else
      raise "AsyncEngine.trap_signal returns false when setting signal `#{signal_name}' (number #{signal_number})"
    end
  end

end


AE = AsyncEngine