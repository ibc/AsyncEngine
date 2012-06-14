module AsyncEngine::Utils

  class NetworkInterface
    attr_reader :name, :ip_type, :ip

    def internal?
      @is_internal
    end

    alias :orig_to_s :to_s
    def to_s
      "#{self.orig_to_s} (name: #{@name}, internal?: #{@is_internal.inspect}, ip: #{@ip})"
    end
    alias :inspect :to_s
  end

  class CpuInfo
    attr_reader :model, :speed
    attr_reader :time_sys, :time_user, :time_idle, :time_irq, :time_nice

    alias :orig_to_s :to_s
    def to_s
      "#{self.orig_to_s} (model: #{@model.inspect}, speed: #{speed} MHz)"
    end
    alias :inspect :to_s
  end

end
