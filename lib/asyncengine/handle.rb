module AsyncEngine

  class Handle
    def closed?
      ! alive?
    end
  end

end
