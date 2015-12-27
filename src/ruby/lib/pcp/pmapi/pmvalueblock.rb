require 'pcp_native'

module PCP
  class PMAPI
    class PmValueBlock

      def inspect
        "#<#{self.class.name}: vlen=#{vlen} vtype=#{vtype} vbuf=#{vbuf}>"
      end

      def ==(other)
        self.class == other.class && \
        vlen == other.vlen && \
        vtype == other.vtype && \
        vbuf == other.vbuf
      end

    end
  end
end