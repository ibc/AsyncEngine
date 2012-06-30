module AsyncEngine
  module Version
    MAJOR = 0
    MINOR = 0
    TINY  = 2
    DEVEL = "alpha1"  # Set to nil for stable releases.
  end

  VERSION = [Version::MAJOR, Version::MINOR, Version::TINY].join(".")
  VERSION << ".#{Version::DEVEL}"  if Version::DEVEL
end