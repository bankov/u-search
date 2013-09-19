package ru.mipt.smbsearch.beans;

import java.util.List;

import javax.faces.bean.ManagedBean;

import ru.mipt.smbsearch.database.MssFileLocator;
import ru.mipt.smbsearch.database.entities.MssFile;

@ManagedBean(name = "search")
public class Search {
	private String query;
	private List<MssFile> findedValues;
	
	public String getQuery() {
		return query;
	}

	public void setQuery(String query) {
		this.query = query;
	}

	public List<MssFile> getFindedValues() {
		return findedValues;
	}

	public void setFindedValues(List<MssFile> findedValues) {
		this.findedValues = findedValues;
	}	
	
	public void search() {
		findedValues = MssFileLocator.findFilesByNameAndServer(query, "natalie", 0, 10);
	}

}
