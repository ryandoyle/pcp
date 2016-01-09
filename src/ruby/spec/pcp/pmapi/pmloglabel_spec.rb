require 'spec_helper'

describe PCP::PMAPI::PmLogLabel do
  describe '#new' do
    it 'should raise an error because construction via Ruby is not supported' do
      expect{described_class.new}.to raise_error NotImplementedError
    end
  end
end