//
// Created by soegaard on 8/23/17.
//

#include <iostream>
#include <sstream>

#include <mbgen/MBArgs.h>
#include <mbcommon/TextFile.h>
#include <libs/include/Socket.h>
#include <iomanip>
#include <cstring>

int main(int argc, char *argv[]) {
    MBArgs opts(argc, argv);

    Socket::Endpoint local("0.0.0.0", 0);
    Socket::Endpoint remote(opts.dest_ip.c_str(), opts.port);

    UDPClient DataSource(local, remote);
    DataSource.buflen(opts.buflen);
    DataSource.setbuffers(opts.sndbuf, 0);
    DataSource.printbuffers();

    TextFile file(opts.filename);

    char buffer[9000];
    char *bufptr = buffer;

    uint64_t pkt = 0;
    uint64_t bytes = 0;

    uint linesize = sizeof(TextFile::Entry::digi) + sizeof(TextFile::Entry::chan) + sizeof(TextFile::Entry::adc) +
                    sizeof(TextFile::Entry::time);

    TextFile::Entry entry = {0, 0, 0, 0};
    TextFile::Entry end   = {UINT8_MAX, UINT8_MAX, UINT16_MAX, UINT32_MAX};

    uint dppkg = 0;

    int pkgsize = 0;

    do {

        try {
            entry = file.nextEntry();
        } catch (TextFile::eof e) {

            std::cout << "End of file reached." << std::endl;

            break;
        }

        std::memcpy(bufptr, &entry.digi, sizeof(TextFile::Entry::digi));
        bufptr += sizeof(TextFile::Entry::digi);

        std::memcpy(bufptr, &entry.chan, sizeof(TextFile::Entry::chan));
        bufptr += sizeof(TextFile::Entry::chan);

        std::memcpy(bufptr, &entry.adc, sizeof(TextFile::Entry::adc));
        bufptr += sizeof(TextFile::Entry::adc);

        std::memcpy(bufptr, &entry.time, sizeof(TextFile::Entry::time));
        bufptr += sizeof(TextFile::Entry::time);

        pkgsize += linesize;

        dppkg++;

        if (!(dppkg < opts.dppkg) || !((pkgsize + linesize) < opts.buflen)) {

            DataSource.send(buffer, pkgsize);

            bytes += pkgsize;//buflen;
            pkt++;

            bufptr = buffer;

            pkgsize = 0;
            dppkg = 0;
        }

    } while (pkt < opts.txPkt);

    std::memcpy(bufptr, &end.digi, sizeof(TextFile::Entry::digi));
    bufptr += sizeof(TextFile::Entry::digi);

    std::memcpy(bufptr, &end.chan, sizeof(TextFile::Entry::chan));
    bufptr += sizeof(TextFile::Entry::chan);

    std::memcpy(bufptr, &end.adc, sizeof(TextFile::Entry::adc));
    bufptr += sizeof(TextFile::Entry::adc);

    std::memcpy(bufptr, &end.time, sizeof(TextFile::Entry::time));
    bufptr += sizeof(TextFile::Entry::time);

    pkgsize += linesize;

    DataSource.send(buffer, pkgsize);

    bytes += pkgsize;//buflen;
    pkt++;

    printf("Sent: %" PRIu64 " packets\n", pkt);
    printf("Sent: %" PRIu64 " bytes\n", bytes);

    if (opts.outfile.empty() || opts.filename.empty())
        return 0;

    printf("Success creating\n");

    return 0;
}