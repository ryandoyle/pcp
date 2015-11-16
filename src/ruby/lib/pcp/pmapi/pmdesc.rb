require 'pcp/pmapi'

module PCP
  class PMAPI
    class PmDesc
      def initialize(pmid, type, indom, sem, units)
        self.pmid = pmid
        self.type = type
        self.indom = indom
        self.sem = sem
        raise ::TypeError, "#{units.class} are invalid units. Must be of type PCP::PMAPI::PmUnits" unless units.is_a?(PCP::PMAPI::PmUnits)
        @units = units
      end

      attr_reader :units

      def ==(other)
        self.class == other.class && \
        pmid == other.pmid && \
        type == other.type && \
        indom == other.indom && \
        sem == other.sem && \
        units == other.units
      end

      def inspect
        "#<#{self.class.name}: pmid=#{pmid} type=#{type} indom=#{indom} sem=#{sem} units=#{units.inspect}>"
      end

    end
  end
end
