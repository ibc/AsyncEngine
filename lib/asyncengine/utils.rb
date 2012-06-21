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
      "#{self.orig_to_s} (model: #{@model}, speed: #{speed} MHz)"
    end

    def inspect
      sprintf("%s sys: %.2f%%, user: %.2f%%, idle: %.2f%%, irq: %.2f%%, nice: %.2f%%",
              to_s, @time_sys, @time_user, @time_idle, @time_irq, @time_nice)
    end
  end

end
