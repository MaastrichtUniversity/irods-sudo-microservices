/**
 * \file
 * \brief     Group member remove sudo microservice.
 * \author    Chris Smeele
 * \copyright Copyright (c) 2016, Utrecht University. All rights reserved.
 */
#include "common.hh"
#include <generalAdmin.h>

namespace Sudo {
    int groupMemberRemove(ruleExecInfo_t *rei,
                          msParam_t *groupName_,
                          msParam_t *userName_,
                          msParam_t *policyKv_) {

        if (std::string(groupName_->type) != STR_MS_T) {
            std::cerr << __FILE__ << ": Group name must be a string.\n";
            return SYS_INVALID_INPUT_PARAM;
        }
        const std::string groupName = parseMspForStr(groupName_);

        if (std::string(userName_->type) != STR_MS_T) {
            std::cerr << __FILE__ << ": User name must be a string.\n";
            return SYS_INVALID_INPUT_PARAM;
        }
        const std::string userStr = parseMspForStr(userName_);

        std::string userName, zoneName;
        std::tie(userName, zoneName) = splitUserZone(userStr, rei);

        generalAdminInp_t adminParams = { };

        adminParams.arg0 = const_cast<char*>("modify");
        adminParams.arg1 = const_cast<char*>("group");
        adminParams.arg2 = const_cast<char*>(groupName.c_str());
        adminParams.arg3 = const_cast<char*>("remove");
        adminParams.arg4 = const_cast<char*>(userName.c_str());
        adminParams.arg5 = const_cast<char*>(zoneName.c_str());
        adminParams.arg6 = const_cast<char*>("");
        adminParams.arg7 = const_cast<char*>("");
        adminParams.arg8 = const_cast<char*>("");
        adminParams.arg9 = const_cast<char*>("");

        return sudo(rei, std::bind<int>(rsGeneralAdmin, rei->rsComm, &adminParams));
    }
}

extern "C" {
    int msiSudoGroupMemberRemove(msParam_t *groupName_,
                                 msParam_t *userName_,
                                 msParam_t *policyKv_,
                                 ruleExecInfo_t *rei) {

        return Sudo::policify("SudoGroupMemberRemove",
                              Sudo::msi_3param_t(Sudo::groupMemberRemove),
                              rei,
                              groupName_,
                              userName_,
                              policyKv_);
    }

    irods::ms_table_entry* plugin_factory() {

        irods::ms_table_entry* msvc = new irods::ms_table_entry(3);

        // C symbol, rule symbol.
        msvc->add_operation("msiSudoGroupMemberRemove",
                            "msiSudoGroupMemberRemove");
        return msvc;
    }
}
