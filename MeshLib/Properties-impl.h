/**
 * \file
 * \brief  Implemenatiom of the template part of the class Properties.
 *
 * \copyright
 * Copyright (c) 2012-2015, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */


template <typename T>
boost::optional<PropertyVector<T> &>
Properties::createNewPropertyVector(std::string const& name,
	MeshItemType mesh_item_type,
	std::size_t tuple_size)
{
	std::map<std::string, PropertyVectorBase*>::const_iterator it(
		_properties.find(name)
	);
	if (it != _properties.end()) {
		ERR("A property of the name \"%s\" is already assigned to the mesh.",
			name.c_str());
		return boost::optional<PropertyVector<T> &>();
	}
	auto entry_info(
		_properties.insert(
			std::make_pair(
				name, new PropertyVector<T>(name, mesh_item_type, tuple_size)
			)
		)
	);
	return boost::optional<PropertyVector<T> &>(*(
			static_cast<PropertyVector<T>*>((entry_info.first)->second)
		)
	);
}

template <typename T>
boost::optional<PropertyVector<T> &>
Properties::createNewPropertyVector(std::string const& name,
	std::size_t n_prop_groups,
	std::vector<std::size_t> const& item2group_mapping,
	MeshItemType mesh_item_type,
	std::size_t tuple_size)
{
	// check if there is already a PropertyVector with the same name and
	// mesh_item_type
	std::map<std::string, PropertyVectorBase*>::const_iterator it(
		_properties.find(name)
	);
	if (it != _properties.end()) {
		ERR("A property of the name \"%s\" already assigned to the mesh.",
			name.c_str());
		return boost::optional<PropertyVector<T> &>();
	}

	// check entries of item2group_mapping of consistence
	for (std::size_t k(0); k<item2group_mapping.size(); k++) {
		std::size_t const group_id (item2group_mapping[k]);
		if (group_id >= n_prop_groups) {
			ERR("The mapping to property %d for item %d is not in the correct range [0,%d).", group_id, k, n_prop_groups);
			return boost::optional<PropertyVector<T> &>();
		}
	}

	auto entry_info(
		_properties.insert(
			std::pair<std::string, PropertyVectorBase*>(
				name,
				new PropertyVector<T>(n_prop_groups,
					item2group_mapping, name, mesh_item_type, tuple_size)
			)
		)
	);
	return boost::optional<PropertyVector<T> &>
		(*(static_cast<PropertyVector<T>*>((entry_info.first)->second)));
}

template <typename T>
boost::optional<PropertyVector<T> const&>
Properties::getPropertyVector(std::string const& name) const
{
	std::map<std::string, PropertyVectorBase*>::const_iterator it(
		_properties.find(name)
	);
	if (it == _properties.end()) {
		ERR("A property with the specified name \"%s\" is not available.",
			name.c_str());
		return boost::optional<PropertyVector<T> const&>();
	}

	PropertyVector<T> const* t=dynamic_cast<PropertyVector<T>const*>(it->second);
	if (!t) {
		return boost::optional<PropertyVector<T> const&>();
	}
	return *t;
}

template <typename T>
boost::optional<PropertyVector<T>&>
Properties::getPropertyVector(std::string const& name)
{
	std::map<std::string, PropertyVectorBase*>::iterator it(
		_properties.find(name)
	);
	if (it == _properties.end()) {
		ERR("A property with the specified name \"%s\" is not available.",
			name.c_str());
		return boost::optional<PropertyVector<T>&>();
	}

	PropertyVector<T> *t=dynamic_cast<PropertyVector<T>*>(it->second);
	if (!t) {
		return boost::optional<PropertyVector<T> &>();
	}
	return *t;
}

