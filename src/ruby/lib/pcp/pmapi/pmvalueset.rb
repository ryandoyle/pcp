require 'pcp/pmapi'

module PCP
  class PMAPI
    class PmValueSet

      def ==(other)
        self.class == other.class && \
        pmid == other.pmid && \
        valfmt == other.valfmt && \
        numval == other.numval && \
        vlist == other.vlist
      end

      def inspect
        "#<#{self.class.name}: pmid=#{pmid} numval=#{numval} valfmt=#{valfmt} vlist=#{vlist}>"
      end

    end
  end
end