package ru.mipt.smbsearch.database.entities;

import java.io.Serializable;
import javax.persistence.*;
import java.util.List;


/**
 * The persistent class for the mss_attributes database table.
 * 
 */
@Entity
@Table(name="mss_attributes")
public class MssAttribute implements Serializable {
	private static final long serialVersionUID = 1L;

	@Id
	@GeneratedValue(strategy=GenerationType.AUTO)
	private int id;

	private String name;

	private String type;

	//bi-directional many-to-one association to MssParameter
	@OneToMany(mappedBy="mssAttribute")
	private List<MssParameter> mssParameters;

	public MssAttribute() {
	}

	public int getId() {
		return this.id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public String getName() {
		return this.name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getType() {
		return this.type;
	}

	public void setType(String type) {
		this.type = type;
	}

	public List<MssParameter> getMssParameters() {
		return this.mssParameters;
	}

	public void setMssParameters(List<MssParameter> mssParameters) {
		this.mssParameters = mssParameters;
	}

	public MssParameter addMssParameter(MssParameter mssParameter) {
		getMssParameters().add(mssParameter);
		mssParameter.setMssAttribute(this);

		return mssParameter;
	}

	public MssParameter removeMssParameter(MssParameter mssParameter) {
		getMssParameters().remove(mssParameter);
		mssParameter.setMssAttribute(null);

		return mssParameter;
	}

}
