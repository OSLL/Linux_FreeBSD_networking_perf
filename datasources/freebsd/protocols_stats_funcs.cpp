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

ProtocolStats ip6_stats(char *_data) {

    ProtocolStats stats;
    ip6stat *data = (ip6stat*)_data;

    ADD_STAT(Ip6sTotal, ip6s_total);
    ADD_STAT(Ip6sTooshort, ip6s_tooshort);
    ADD_STAT(Ip6sToosmall, ip6s_toosmall);
    ADD_STAT(Ip6sFragments, ip6s_fragments);
    ADD_STAT(Ip6sFragdropped, ip6s_fragdropped);
    ADD_STAT(Ip6sFragtimeout, ip6s_fragtimeout);
    ADD_STAT(Ip6sFragoverflow, ip6s_fragoverflow);
    ADD_STAT(Ip6sForward, ip6s_forward);
    ADD_STAT(Ip6sCantforward, ip6s_cantforward);
    ADD_STAT(Ip6sRedirectsent, ip6s_redirectsent);
    ADD_STAT(Ip6sDelivered, ip6s_delivered);
    ADD_STAT(Ip6sLocalout, ip6s_localout);
    ADD_STAT(Ip6sOdropped, ip6s_odropped);
    ADD_STAT(Ip6sReassembled, ip6s_reassembled);
    ADD_STAT(Ip6sAtomicfrags, ip6s_atomicfrags);
    ADD_STAT(Ip6sFragmented, ip6s_fragmented);
    ADD_STAT(Ip6sOfragments, ip6s_ofragments);
    ADD_STAT(Ip6sCantfrag, ip6s_cantfrag);
    ADD_STAT(Ip6sBadoptions, ip6s_badoptions);
    ADD_STAT(Ip6sNoroute, ip6s_noroute);
    ADD_STAT(Ip6sBadvers, ip6s_badvers);
    ADD_STAT(Ip6sRawout, ip6s_rawout);
    ADD_STAT(Ip6sBadscope, ip6s_badscope);
    ADD_STAT(Ip6sNotmember, ip6s_notmember);
    ADD_STAT(Ip6sM1, ip6s_m1);
    ADD_STAT(Ip6sMext1, ip6s_mext1);
    ADD_STAT(Ip6sMext2m, ip6s_mext2m);
    ADD_STAT(Ip6sExthdrtoolong, ip6s_exthdrtoolong);
    ADD_STAT(Ip6sNogif, ip6s_nogif);
    ADD_STAT(Ip6sToomanyhdr, ip6s_toomanyhdr);

    return stats;
}

ProtocolStats rip6_stats(char *_data) {

    ProtocolStats stats;
    rip6stat *data = (rip6stat*)_data;

    ADD_STAT(Rip6sIpackets, rip6s_ipackets);
    ADD_STAT(Rip6sIsum, rip6s_isum);
    ADD_STAT(Rip6sBadsum, rip6s_badsum);
    ADD_STAT(Rip6sNosock, rip6s_nosock);
    ADD_STAT(Rip6sNosockmcast, rip6s_nosockmcast);
    ADD_STAT(Rip6sFullsock, rip6s_fullsock);
    ADD_STAT(Rip6sOpackets, rip6s_opackets);

    return stats;
}

ProtocolStats icmp6_stats(char *_data) {

    ProtocolStats stats;
    icmp6stat *data = (icmp6stat*)_data;

    ADD_STAT(Icp6sError, icp6s_error);
    ADD_STAT(Icp6sCanterror, icp6s_canterror);
    ADD_STAT(Icp6sToofreq, icp6s_toofreq);
    ADD_STAT(Icp6sBadcode, icp6s_badcode);
    ADD_STAT(Icp6sTooshort, icp6s_tooshort);
    ADD_STAT(Icp6sChecksum, icp6s_checksum);
    ADD_STAT(Icp6sBadlen, icp6s_badlen);
    ADD_STAT(Icp6sReflect, icp6s_reflect);
    ADD_STAT(Icp6sNdToomanyopt, icp6s_nd_toomanyopt);
    ADD_STAT(Icp6sPmtuchg, icp6s_pmtuchg);
    ADD_STAT(Icp6sNdBadopt, icp6s_nd_badopt);
    ADD_STAT(Icp6sBadns, icp6s_badns);
    ADD_STAT(Icp6sBadna, icp6s_badna);
    ADD_STAT(Icp6sBadrs, icp6s_badrs);
    ADD_STAT(Icp6sBadra, icp6s_badra);
    ADD_STAT(Icp6sBadredirect, icp6s_badredirect);
    ADD_STAT(Icp6sOverflowdefrtr, icp6s_overflowdefrtr);
    ADD_STAT(Icp6sOverflowprfx, icp6s_overflowprfx);
    ADD_STAT(Icp6sOverflownndp, icp6s_overflownndp);
    ADD_STAT(Icp6sOverflowredirect, icp6s_overflowredirect);
    ADD_STAT(Icp6sInvlhlim, icp6s_invlhlim);
    ADD_STAT(Icp6sOdstUnreachNoroute, icp6s_odst_unreach_noroute);
    ADD_STAT(Icp6sOdstUnreachAdmin, icp6s_odst_unreach_admin);
    ADD_STAT(Icp6sOdstUnreachBeyondscope, icp6s_odst_unreach_beyondscope);
    ADD_STAT(Icp6sOdstUnreachAddr, icp6s_odst_unreach_addr);
    ADD_STAT(Icp6sOdstUnreachNoport, icp6s_odst_unreach_noport);
    ADD_STAT(Icp6sOpacketTooBig, icp6s_opacket_too_big);
    ADD_STAT(Icp6sOtimeExceedTransit, icp6s_otime_exceed_transit);
    ADD_STAT(Icp6sOtimeExceedReassembly, icp6s_otime_exceed_reassembly);
    ADD_STAT(Icp6sOparamprobHeader, icp6s_oparamprob_header);
    ADD_STAT(Icp6sOparamprobNextheader, icp6s_oparamprob_nextheader);
    ADD_STAT(Icp6sOparamprobOption, icp6s_oparamprob_option);
    ADD_STAT(Icp6sOredirect, icp6s_oredirect);
    ADD_STAT(Icp6sOunknown, icp6s_ounknown);

    return stats;
}
