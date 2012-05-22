module AsyncEngine

  # TODO: ¿Simplificar?
  # TODO: Si no está corriendo NO enviar ya que uv_async no hace nada (pero tampoco falla).
  def self.call_from_other_thread pr=nil, &bl
    _c_call_from_other_thread pr || bl
  end

  class << self
    private :_c_call_from_other_thread
  end

end