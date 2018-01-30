# SDKLT
SDKLT (Logical Table Software Development Kit) is a new, powerful, and feature rich Software Development Kit for Broadcom switches. SDKLT provides a new approach to switch configuration using [Logical Tables](https://github.com/Broadcom-Network-Switching-Software/SDKLT/wiki/What-is-a-Logical-Table%3F).  The [Wiki](https://github.com/Broadcom-Network-Switching-Software/SDKLT/wiki) provides an [overview](https://github.com/Broadcom-Network-Switching-Software/SDKLT/wiki/Architecture-Overview) of SDKLT usage and its achitecture.  An architectural feature summary can be found [here](https://github.com/Broadcom-Network-Switching-Software/SDKLT/wiki/Architecture-Feature-Summary). 

SDKLT is a part of the [OpenNSL](https://github.com/Broadcom-Network-Switching-Software/OpenNSL-Tool-Suite) suite.

# Purpose
This release is intended to introduce users to the benefits of SDKLT, the API, and the CLI. Using the demonstration application (Demo App) you will learn how Logical Tables are used for network switch programming.  The CLI shell in the Demo App provides an interactive interface for exploring the Logical Table controls on a Broadcom Tomahawk switch.  The built in C language interpreter (CINT) provides an interactive way to write C code for configuring the Logical Tables, using Logical Table C language API calls.

# Getting Started
Clone a copy of the repository, build the Demo App and follow the example CLIs scipts as they demonstrate the capability of the Logical Table Interface.  The Demo App allows you to become familar with the Tomahawk Logical Table interface, see building the [Demo App](https://github.com/Broadcom-Network-Switching-Software/SDKLT/wiki/Building-the-Demo-App).  The Demo App supports switch configuration via the shell CLI or C APIs.  The CLI reference [guide](https://broadcom-network-switching-software.github.io/CLI_Reference/) provides information on using the shell CLI and introduces the user to the CLIs used to configure Logical Tables. 

# Release Contents
SDKLT source code, Logical Tables, documentation and [examples](https://github.com/Broadcom-Network-Switching-Software/SDKLT/tree/master/examples/bcm56960_a0) for a Broadcom Tomahawk switch device.
The package includes an extensible CLI shell which can be used to interact with the core SDK.
The release will provide users a hands-on experience with the SDKLT using a logical table approach to switch software programming.  The [src](https://github.com/Broadcom-Network-Switching-Software/SDKLT/tree/master/src) directory contains the source code needed to build the Demo App image which targets a [simulated](https://github.com/Broadcom-Network-Switching-Software/SDKLT/wiki/XGSSIM) Broadcom BCM56960 Tomahawk device or an actual Tomahawk device.  Note that no external PHY drivers are part of the source code repository.
The list of SDKLT Tomahawk features is [here](https://github.com/Broadcom-Network-Switching-Software/SDKLT/wiki/Tomahawk-Feature-List).

# Documentation

## APIs Internal and External
Overall [API Reference Guide](https://broadcom-network-switching-software.github.io/Documentation/doc/html/index.html) shows both external and internal APIs.

APIs for [Logical Tables](https://broadcom-network-switching-software.github.io/Documentation/bcmlt/html/index.html) operations

Packet I/O [APIs](https://broadcom-network-switching-software.github.io/Documentation/bcmpkt/html/index.html)

SDK Management contains [APIs](https://broadcom-network-switching-software.github.io/Documentation/bcmmgmt/html/index.html) for starting and stopping the SDK.

## CLI
Basic CLI shell [Commands](https://broadcom-network-switching-software.github.io/Documentation/bcma/html/bcma_clicmd.html)

CLI Commands for [Logical Tables](https://broadcom-network-switching-software.github.io/Documentation/bcma/html/bcma_bcmltcmd.html)

CLI Reference [Guide](https://broadcom-network-switching-software.github.io/CLI_Reference/)

## Example Usage
Reference code and CLI scripts are contained in the examples directory.   

## Logical Table Definitions
Definitions of the Logical Tables device controls used on Tomahawk are provided [here](https://broadcom-network-switching-software.github.io/Logical_Table_Documentation_Guide/).  Click on the blue "Feature Categories" tab to see the main Logical Table groupings such as Port, VLAN, L2 or L3.  Select the L2 logical tables to view all the L2 tables supported by this device.  In order to view the specific L2 logical table controls, click on the table name (e.g. L2_MY_STATION).

# Feedback or Questions
Feedback or questions should be sent to sdklt-feedback.pdl@broadcom.com.

# License and Legal Terms
The software content in this repository is divided into two types, open source and limited open source, each with its own respective license.

The open source software is licensed under the Apache License, version 2.0. A copy of this license is located [here](https://github.com/Broadcom-Network-Switching-Software/SDKLT/blob/master/Legal/Apache2_license).

The limited open source software is licensed under the Broadcom Switch Software License. A copy of this license is located [here](https://github.com/Broadcom-Network-Switching-Software/SDKLT/blob/master/Legal/Broadcom_Switch_Software_license).

The applicable license will be identified at the beginning of the software file.
SDKLT uses some third-party externally licensed components which are listed [here](https://github.com/Broadcom-Network-Switching-Software/SDKLT/wiki/Third-Party-Software-Licenses).
