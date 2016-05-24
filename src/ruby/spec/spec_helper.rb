require 'pcp/pmapi'

RSpec.configure do |c|
  c.include PCP::PMAPI::CTest
end

# Ruby does not support C float so precision is lost going from a double (which is
# supported in Ruby) to a float (which is stored in the value block). This matcher deals
# with the loss of precision in the conversion, making sure it is still within one epsilon
FLOAT_EPSILON = 0.00001
RSpec::Matchers.define :be_as_close_as_possible_to do |expected|
  match do |actual|
    (actual + FLOAT_EPSILON > expected) ^ (actual - FLOAT_EPSILON > expected)
  end

  failure_message do |actual|
    "float #{actual} is greater than #{FLOAT_EPSILON} +/- from #{expected}"
  end
end

RSpec::Matchers.define :be_in_timezone do |expected|
  match do |actual|
    actual.pmWhichZone == expected
  end

  failure_message do |actual|
    "expected pmapi to be in the timezone '#{expected}' but got '#{actual.pmWhichZone}'"
  end

end