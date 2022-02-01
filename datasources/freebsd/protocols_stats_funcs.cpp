//
// Created by shenk on 01.02.2022.
//

#include "protocols_stats_funcs.h"

ProtocolStats ip_stats(char *_data) {

    ProtocolStats stats;
    ipstat *data = (ipstat*)_data;

    ADD_STAT(IpsTotal, ips_total);
    ADD_STAT(IpsBadsum, ips_badsum);
    ADD_STAT(IpsTooshort, ips_tooshort);
    ADD_STAT(IpsToosmall, ips_toosmall);
    ADD_STAT(IpsBadhlen, ips_badhlen);
    ADD_STAT(IpsBadlen, ips_badlen);
    ADD_STAT(IpsFragments, ips_fragments);
    ADD_STAT(IpsFragdropped, ips_fragdropped);
    ADD_STAT(IpsFragtimeout, ips_fragtimeout);
    ADD_STAT(IpsForward, ips_forward);
    ADD_STAT(IpsFastforward, ips_fastforward);
    ADD_STAT(IpsCantforward, ips_cantforward);
    ADD_STAT(IpsRedirectsent, ips_redirectsent);
    ADD_STAT(IpsNoproto, ips_noproto);
    ADD_STAT(IpsDelivered, ips_delivered);
    ADD_STAT(IpsLocalout, ips_localout);
    ADD_STAT(IpsOdropped, ips_odropped);
    ADD_STAT(IpsReassembled, ips_reassembled);
    ADD_STAT(IpsFragmented, ips_fragmented);
    ADD_STAT(IpsOfragments, ips_ofragments);
    ADD_STAT(IpsCantfrag, ips_cantfrag);
    ADD_STAT(IpsBadoptions, ips_badoptions);
    ADD_STAT(IpsNoroute, ips_noroute);
    ADD_STAT(IpsBadvers, ips_badvers);
    ADD_STAT(IpsRawout, ips_rawout);
    ADD_STAT(IpsToolong, ips_toolong);
    ADD_STAT(IpsNotmember, ips_notmember);
    ADD_STAT(IpsNogif, ips_nogif);
    ADD_STAT(IpsBadaddr, ips_badaddr);

    return stats;
}


ProtocolStats tcp_stats(char *_data) {

    ProtocolStats stats;
    tcpstat *data = (tcpstat*)_data;

    ADD_STAT(TcpsConnattempt, tcps_connattempt);
    ADD_STAT(TcpsAccepts, tcps_accepts);
    ADD_STAT(TcpsConnects, tcps_connects);
    ADD_STAT(TcpsDrops, tcps_drops);
    ADD_STAT(TcpsConndrops, tcps_conndrops);
    ADD_STAT(TcpsMinmssdrops, tcps_minmssdrops);
    ADD_STAT(TcpsClosed, tcps_closed);
    ADD_STAT(TcpsSegstimed, tcps_segstimed);
    ADD_STAT(TcpsRttupdated, tcps_rttupdated);
    ADD_STAT(TcpsDelack, tcps_delack);
    ADD_STAT(TcpsTimeoutdrop, tcps_timeoutdrop);
    ADD_STAT(TcpsRexmttimeo, tcps_rexmttimeo);
    ADD_STAT(TcpsPersisttimeo, tcps_persisttimeo);
    ADD_STAT(TcpsKeeptimeo, tcps_keeptimeo);
    ADD_STAT(TcpsKeepprobe, tcps_keepprobe);
    ADD_STAT(TcpsKeepdrops, tcps_keepdrops);
    ADD_STAT(TcpsSndtotal, tcps_sndtotal);
    ADD_STAT(TcpsSndpack, tcps_sndpack);
    ADD_STAT(TcpsSndbyte, tcps_sndbyte);
    ADD_STAT(TcpsSndrexmitpack, tcps_sndrexmitpack);
    ADD_STAT(TcpsSndrexmitbyte, tcps_sndrexmitbyte);
    ADD_STAT(TcpsSndrexmitbad, tcps_sndrexmitbad);
    ADD_STAT(TcpsSndacks, tcps_sndacks);
    ADD_STAT(TcpsSndprobe, tcps_sndprobe);
    ADD_STAT(TcpsSndurg, tcps_sndurg);
    ADD_STAT(TcpsSndwinup, tcps_sndwinup);
    ADD_STAT(TcpsSndctrl, tcps_sndctrl);
    ADD_STAT(TcpsRcvtotal, tcps_rcvtotal);
    ADD_STAT(TcpsRcvpack, tcps_rcvpack);
    ADD_STAT(TcpsRcvbyte, tcps_rcvbyte);
    ADD_STAT(TcpsRcvbadsum, tcps_rcvbadsum);
    ADD_STAT(TcpsRcvbadoff, tcps_rcvbadoff);
    ADD_STAT(TcpsRcvreassfull, tcps_rcvreassfull);
    ADD_STAT(TcpsRcvshort, tcps_rcvshort);
    ADD_STAT(TcpsRcvduppack, tcps_rcvduppack);
    ADD_STAT(TcpsRcvdupbyte, tcps_rcvdupbyte);
    ADD_STAT(TcpsRcvpartduppack, tcps_rcvpartduppack);
    ADD_STAT(TcpsRcvpartdupbyte, tcps_rcvpartdupbyte);
    ADD_STAT(TcpsRcvoopack, tcps_rcvoopack);
    ADD_STAT(TcpsRcvoobyte, tcps_rcvoobyte);
    ADD_STAT(TcpsRcvpackafterwin, tcps_rcvpackafterwin);
    ADD_STAT(TcpsRcvbyteafterwin, tcps_rcvbyteafterwin);
    ADD_STAT(TcpsRcvafterclose, tcps_rcvafterclose);
    ADD_STAT(TcpsRcvwinprobe, tcps_rcvwinprobe);
    ADD_STAT(TcpsRcvdupack, tcps_rcvdupack);
    ADD_STAT(TcpsRcvacktoomuch, tcps_rcvacktoomuch);
    ADD_STAT(TcpsRcvackpack, tcps_rcvackpack);
    ADD_STAT(TcpsRcvackbyte, tcps_rcvackbyte);
    ADD_STAT(TcpsRcvwinupd, tcps_rcvwinupd);
    ADD_STAT(TcpsPawsdrop, tcps_pawsdrop);
    ADD_STAT(TcpsPredack, tcps_predack);
    ADD_STAT(TcpsPreddat, tcps_preddat);
    ADD_STAT(TcpsPcbcachemiss, tcps_pcbcachemiss);
    ADD_STAT(TcpsCachedrtt, tcps_cachedrtt);
    ADD_STAT(TcpsCachedrttvar, tcps_cachedrttvar);
    ADD_STAT(TcpsCachedssthresh, tcps_cachedssthresh);
    ADD_STAT(TcpsUsedrtt, tcps_usedrtt);
    ADD_STAT(TcpsUsedrttvar, tcps_usedrttvar);
    ADD_STAT(TcpsUsedssthresh, tcps_usedssthresh);
    ADD_STAT(TcpsPersistdrop, tcps_persistdrop);
    ADD_STAT(TcpsBadsyn, tcps_badsyn);
    ADD_STAT(TcpsMturesent, tcps_mturesent);
    ADD_STAT(TcpsListendrop, tcps_listendrop);
    ADD_STAT(TcpsBadrst, tcps_badrst);
    ADD_STAT(TcpsScAdded, tcps_sc_added);
    ADD_STAT(TcpsScRetransmitted, tcps_sc_retransmitted);
    ADD_STAT(TcpsScDupsyn, tcps_sc_dupsyn);
    ADD_STAT(TcpsScDropped, tcps_sc_dropped);
    ADD_STAT(TcpsScCompleted, tcps_sc_completed);
    ADD_STAT(TcpsScBucketoverflow, tcps_sc_bucketoverflow);
    ADD_STAT(TcpsScCacheoverflow, tcps_sc_cacheoverflow);
    ADD_STAT(TcpsScReset, tcps_sc_reset);
    ADD_STAT(TcpsScStale, tcps_sc_stale);
    ADD_STAT(TcpsScAborted, tcps_sc_aborted);
    ADD_STAT(TcpsScBadack, tcps_sc_badack);
    ADD_STAT(TcpsScUnreach, tcps_sc_unreach);
    ADD_STAT(TcpsScZonefail, tcps_sc_zonefail);
    ADD_STAT(TcpsScSendcookie, tcps_sc_sendcookie);
    ADD_STAT(TcpsScRecvcookie, tcps_sc_recvcookie);
    ADD_STAT(TcpsHcAdded, tcps_hc_added);
    ADD_STAT(TcpsHcBucketoverflow, tcps_hc_bucketoverflow);
    ADD_STAT(TcpsFinwait2Drops, tcps_finwait2_drops);
    ADD_STAT(TcpsSackRecoveryEpisode, tcps_sack_recovery_episode);
    ADD_STAT(TcpsSackRexmits, tcps_sack_rexmits);
    ADD_STAT(TcpsSackRexmitBytes, tcps_sack_rexmit_bytes);
    ADD_STAT(TcpsSackRcvBlocks, tcps_sack_rcv_blocks);
    ADD_STAT(TcpsSackSendBlocks, tcps_sack_send_blocks);
//    ADD_STAT(TcpsSackLostrexmt, tcps_sack_lostrexmt);
    ADD_STAT(TcpsSackSboverflow, tcps_sack_sboverflow);
    ADD_STAT(TcpsEcnCe, tcps_ecn_ce);
    ADD_STAT(TcpsEcnEct0, tcps_ecn_ect0);
    ADD_STAT(TcpsEcnEct1, tcps_ecn_ect1);
    ADD_STAT(TcpsEcnShs, tcps_ecn_shs);
    ADD_STAT(TcpsEcnRcwnd, tcps_ecn_rcwnd);
    ADD_STAT(TcpsSigRcvgoodsig, tcps_sig_rcvgoodsig);
    ADD_STAT(TcpsSigRcvbadsig, tcps_sig_rcvbadsig);
    ADD_STAT(TcpsSigErrBuildsig, tcps_sig_err_buildsig);
    ADD_STAT(TcpsSigErrSigopt, tcps_sig_err_sigopt);
    ADD_STAT(TcpsSigErrNosigopt, tcps_sig_err_nosigopt);
    ADD_STAT(TcpsPmtudBlackholeActivated, tcps_pmtud_blackhole_activated);
    ADD_STAT(TcpsPmtudBlackholeActivatedMinMss, tcps_pmtud_blackhole_activated_min_mss);
    ADD_STAT(TcpsPmtudBlackholeFailed, tcps_pmtud_blackhole_failed);
//    ADD_STAT(TcpsTunneledPkts, tcps_tunneled_pkts);
//    ADD_STAT(TcpsTunneledErrs, tcps_tunneled_errs);
//    ADD_STAT(TcpsDsackCount, tcps_dsack_count);
//    ADD_STAT(TcpsDsackBytes, tcps_dsack_bytes);
//    ADD_STAT(TcpsDsackTlpBytes, tcps_dsack_tlp_bytes);

    return stats;
}

ProtocolStats udp_stats(char *_data) {

    ProtocolStats stats;
    udpstat *data = (udpstat*)_data;

    ADD_STAT(UdpsIpackets, udps_ipackets);
    ADD_STAT(UdpsHdrops, udps_hdrops);
    ADD_STAT(UdpsBadsum, udps_badsum);
    ADD_STAT(UdpsNosum, udps_nosum);
    ADD_STAT(UdpsBadlen, udps_badlen);
    ADD_STAT(UdpsNoport, udps_noport);
    ADD_STAT(UdpsNoportbcast, udps_noportbcast);
    ADD_STAT(UdpsFullsock, udps_fullsock);
    ADD_STAT(UdppsPcbcachemiss, udpps_pcbcachemiss);
    ADD_STAT(UdppsPcbhashmiss, udpps_pcbhashmiss);
    ADD_STAT(UdpsOpackets, udps_opackets);
    ADD_STAT(UdpsFastout, udps_fastout);
    ADD_STAT(UdpsNoportmcast, udps_noportmcast);
    ADD_STAT(UdpsFiltermcast, udps_filtermcast);

    return stats;
}

ProtocolStats icmp_stats(char *_data) {

    ProtocolStats stats;
    icmpstat *data = (icmpstat*)_data;

    ADD_STAT(IcpsError, icps_error);
    ADD_STAT(IcpsOldshort, icps_oldshort);
    ADD_STAT(IcpsOldicmp, icps_oldicmp);
    ADD_STAT(IcpsBadcode, icps_badcode);
    ADD_STAT(IcpsTooshort, icps_tooshort);
    ADD_STAT(IcpsChecksum, icps_checksum);
    ADD_STAT(IcpsBadlen, icps_badlen);
    ADD_STAT(IcpsReflect, icps_reflect);
    ADD_STAT(IcpsBmcastecho, icps_bmcastecho);
    ADD_STAT(IcpsBmcasttstamp, icps_bmcasttstamp);
    ADD_STAT(IcpsBadaddr, icps_badaddr);
    ADD_STAT(IcpsNoroute, icps_noroute);

    return stats;
}

ProtocolStats igmp_stats(char *_data) {

    ProtocolStats stats;
    igmpstat *data = (igmpstat*)_data;

    ADD_STAT(IgpsRcvTotal, igps_rcv_total);
    ADD_STAT(IgpsRcvTooshort, igps_rcv_tooshort);
    ADD_STAT(IgpsRcvBadttl, igps_rcv_badttl);
    ADD_STAT(IgpsRcvBadsum, igps_rcv_badsum);
    ADD_STAT(IgpsRcvV1v2Queries, igps_rcv_v1v2_queries);
    ADD_STAT(IgpsRcvV3Queries, igps_rcv_v3_queries);
    ADD_STAT(IgpsRcvBadqueries, igps_rcv_badqueries);
    ADD_STAT(IgpsRcvGenQueries, igps_rcv_gen_queries);
    ADD_STAT(IgpsRcvGroupQueries, igps_rcv_group_queries);
    ADD_STAT(IgpsRcvGsrQueries, igps_rcv_gsr_queries);
    ADD_STAT(IgpsDropGsrQueries, igps_drop_gsr_queries);
    ADD_STAT(IgpsRcvReports, igps_rcv_reports);
    ADD_STAT(IgpsRcvBadreports, igps_rcv_badreports);
    ADD_STAT(IgpsRcvOurreports, igps_rcv_ourreports);
    ADD_STAT(IgpsRcvNora, igps_rcv_nora);
    ADD_STAT(IgpsSndReports, igps_snd_reports);

    return stats;
}
