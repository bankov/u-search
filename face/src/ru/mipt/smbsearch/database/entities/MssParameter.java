package ru.mipt.smbsearch.database.entities;

import java.io.Serializable;
import javax.persistence.*;


/**
 * The persistent class for the mss_parameters database table.
 * 
 */
@Entity
@Table(name="mss_parameters")
public class MssParameter implements Serializable {
	private static final long serialVersionUID = 1L;

	@EmbeddedId
	private MssParameterPK id;

	@Column(name="bool_value")
	private byte boolValue;

	@Column(name="num_value")
	private int numValue;

	@Lob
	@Column(name="str_value")
	private String strValue;

	//bi-directional many-to-one association to MssAttribute
	@ManyToOne
	@JoinColumn(name="attr_id", insertable=false, updatable=false)
	private MssAttribute mssAttribute;

	//bi-directional many-to-one association to MssFile
	@ManyToOne
	@JoinColumn(name="file_id", insertable=false, updatable=false)
	private MssFile mssFile;

	public MssParameter() {
	}

	public MssParameterPK getId() {
		return this.id;
	}

	public void setId(MssParameterPK id) {
		this.id = id;
	}

	public byte getBoolValue() {
		return this.boolValue;
	}

	public void setBoolValue(byte boolValue) {
		this.boolValue = boolValue;
	}

	public int getNumValue() {
		return this.numValue;
	}

	public void setNumValue(int numValue) {
		this.numValue = numValue;
	}

	public String getStrValue() {
		return this.strValue;
	}

	public void setStrValue(String strValue) {
		this.strValue = strValue;
	}

	public MssAttribute getMssAttribute() {
		return this.mssAttribute;
	}

	public void setMssAttribute(MssAttribute mssAttribute) {
		this.mssAttribute = mssAttribute;
	}

	public MssFile getMssFile() {
		return this.mssFile;
	}

	public void setMssFile(MssFile mssFile) {
		this.mssFile = mssFile;
	}

}