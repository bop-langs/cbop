require 'test/unit'

class ZombieTester < Test::Unit::TestCase
  ARGV.each{|exec|
    define_method("test_#{exec}"){
      output = `ps -al | grep #{exec}`
      assert output.empty?
    }
  }
end
