# -*- encoding: utf-8 -*-

require "./lib/asyncengine/version"

Gem::Specification.new do |spec|
  spec.name = "asyncengine"
  spec.version = AsyncEngine::VERSION
  spec.date = Time.now
  spec.authors = ["Inaki Baz Castillo"]
  spec.email = "ibc@aliax.net"
  spec.summary = "Ruby asynchronous event driven framework on top of libuv"
  spec.homepage = "https://github.com/ibc/AsyncEngine"
  spec.description = "Ruby asynchronous event driven framework on top of libuv"
  spec.required_ruby_version = ">= 1.9.2"

  files = %w{
    asyncengine.gemspec
    Rakefile
    README.markdown
    lib/**/**/**
    ext/asyncengine/{*.c,*.h,*.rb}
    ext/asyncengine/libuv/**/**/**/**/**/**/**/**/**
    test/{ae_test_helper.rb}
    test/{test_*.rb}
  }.join(",")

  spec.files = Dir.glob "{#{files}}"
  spec.require_paths = ["lib"]
  spec.extensions = ["ext/asyncengine/extconf.rb"]
end
