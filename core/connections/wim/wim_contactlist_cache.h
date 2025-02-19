#ifndef __WIM_CONTACTLIST_CACHE_H_
#define __WIM_CONTACTLIST_CACHE_H_

#pragma once



namespace core
{
    class async_executer;
    struct icollection;

    namespace wim
    {
        class im;

        class contactlist;

        struct cl_presence
        {
            std::string state_;
            std::string usertype_;
            std::string status_msg_;
            std::string other_number_;
            std::string sms_number_;
            std::set<std::string> capabilities_;
            std::string ab_contact_name_;
            std::string friendly_;
            int32_t lastseen_;
            int32_t outgoing_msg_count_;
            bool is_chat_;
            bool muted_;
            bool is_live_chat_;
            bool official_;
            std::string icon_id_;
            std::string big_icon_id_;
            std::string large_icon_id_;

            struct search_cache
            {
                std::string aimid_;
                std::string friendly_;
                std::string ab_;
                std::string sms_number_;

                std::vector<std::string> friendly_words_;
                std::vector<std::string> ab_words_;

                bool is_empty() const { return aimid_.empty(); }
                void clear() { aimid_.clear(); friendly_.clear(); ab_.clear(); sms_number_.clear();  }

            } search_cache_;

            cl_presence()
                : lastseen_(-1), outgoing_msg_count_(0), is_chat_(false), muted_(false), is_live_chat_(false), official_(false)
            {
            }

            void serialize(icollection* _coll);
            void serialize(rapidjson::Value& _node, rapidjson_allocator& _a);
            void unserialize(const rapidjson::Value& _node);
        };

        struct cl_buddy
        {
            uint32_t id_;
            std::string aimid_;
            std::shared_ptr<cl_presence> presence_;

            cl_buddy() : id_(0), presence_(new cl_presence()) {}
        };


        struct cl_group
        {
            uint32_t id_ = 0;
            std::string name_;

            std::list<std::shared_ptr<cl_buddy>> buddies_;

            bool added_ = false;
            bool removed_ = false;
        };

        typedef std::unordered_set<std::string> ignorelist_cache;

        class contactlist
        {
        public:
            enum class changed_status
            {
                full,
                presence,
                none
            };

        private:
            changed_status changed_status_ = changed_status::none;
            bool need_update_search_cache_ = false;
            bool need_update_avatar_ = false;
            void set_need_update_cache(bool _need_update_search_cache);

            std::list<std::shared_ptr<cl_group>> groups_;

            ignorelist_cache ignorelist_;

        public:

            // TODO : make it private
            std::map<std::string, std::shared_ptr<cl_buddy>> search_cache_;
            std::map<std::string, std::shared_ptr<cl_buddy>> tmp_cache_;
            std::map< std::string, std::shared_ptr<cl_buddy> > contacts_index_;
            std::map<std::string, int32_t> search_priority_;
            std::string last_search_patterns_;

            void update_cl(const contactlist& _cl);
            void update_ignorelist(const ignorelist_cache& _ignorelist);

            void set_changed_status(changed_status _status) noexcept;
            changed_status get_changed_status() const noexcept;

            bool get_need_update_search_cache() const noexcept { return need_update_search_cache_; }
            bool get_need_update_avatar(bool reset) noexcept
            {
                const auto need = need_update_avatar_;
                if (reset)
                    need_update_avatar_ = false;
                return need;
            }

            bool exist(const std::string& contact) const { return contacts_index_.find(contact) != contacts_index_.end(); }

            std::string get_contact_friendly_name(const std::string& contact_login) const;

            int32_t unserialize(const rapidjson::Value& _node);
            int32_t unserialize_from_diff(const rapidjson::Value& _node);

            void serialize(rapidjson::Value& _node, rapidjson_allocator& _a) const;
            void serialize(icollection* _coll, const std::string& type) const;
            void serialize_search(icollection* _coll) const;
            void serialize_ignorelist(icollection* _coll) const;
            void serialize_contact(const std::string& _aimid, icollection* _coll) const;
            std::vector<std::string> search(const std::vector<std::vector<std::string>>& search_patterns, int32_t fixed_patterns_count);
            std::vector<std::string> search(const std::string& search_pattern, bool first, int32_t searh_priority, int32_t fixed_patters_count);

            void update_presence(const std::string& _aimid, const std::shared_ptr<cl_presence>& _presence);
            void merge_from_diff(const std::string& _type, const std::shared_ptr<contactlist>& _diff, const std::shared_ptr<std::list<std::string>>& removedContacts);

            int32_t get_contacts_count() const;
            int32_t get_phone_contacts_count() const;
            int32_t get_groupchat_contacts_count() const;

            void add_to_ignorelist(const std::string& _aimid);
            void remove_from_ignorelist(const std::string& _aimid);

            std::shared_ptr<cl_presence> get_presence(const std::string& _aimid) const;
            void set_outgoing_msg_count(const std::string& _contact, int32_t _count);

            std::shared_ptr<cl_group> get_first_group() const;
            bool is_ignored(const std::string& _aimid) const;
        };
    }
}



#endif //__WIM_CONTACTLIST_CACHE_H_
