require 'pcp/pmapi'

module PCP
  class PMAPI
    class PmDesc

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
